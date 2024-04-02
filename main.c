#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ulfius.h>
#include <jansson.h>

#define PORT 8080
#define FILE_NAME "books.dat"

typedef struct
{
    int id;
    char name[256];
} Book;

void custom_log(const char *level, const char *file, int line, const char *ip, const char *method, const char *url, const char *http_version, int status)
{
    time_t now = time(NULL);
    char formatted_time[20];
    strftime(formatted_time, sizeof(formatted_time), "%Y-%m-%d %H:%M:%S", localtime(&now));

    printf("[%s] [%s] [%s:%d] %s %s %s %s %d OK\n", formatted_time, level, file, line, ip, method, url, http_version, status);
}

#define LOG(level, ip, method, url, http_version, status) custom_log(level, __FILE__, __LINE__, ip, method, url, http_version, status)

void log_request(const struct _u_request *request, const struct _u_response *response)
{
    const char *ip = u_map_get(request->map_header, "X-Real-IP"); // Assuming a reverse proxy sets the real IP
    if (!ip)
    {
        ip = "Unknown IP"; // Fallback if the IP is not set
    }
    const char *method = request->http_verb;
    const char *url = request->http_url;
    const char *http_version = request->http_protocol;
    int status = response->status; // Should be set in your response logic

    LOG("INFO", ip, method, url, http_version, status);
}

// Function Prototypes
int callback_create_book(const struct _u_request *request, struct _u_response *response, void *user_data);
int callback_read_book(const struct _u_request *request, struct _u_response *response, void *user_data);
int callback_update_book(const struct _u_request *request, struct _u_response *response, void *user_data);
int callback_delete_book(const struct _u_request *request, struct _u_response *response, void *user_data);
void add_book_to_file(Book book);
Book *read_all_books_from_file(int *count);
void update_book_in_file(int id, const char *newName);
void delete_book_from_file(int id);

int main(void)
{
    struct _u_instance instance;

    if (ulfius_init_instance(&instance, PORT, NULL, NULL) != U_OK)
    {
        fprintf(stderr, "Error ulfius_init_instance, abort\n");
        return 1;
    }

    // CRUD Endpoint Registrations
    ulfius_add_endpoint_by_val(&instance, "POST", "/book", NULL, 0, &callback_create_book, NULL);
    ulfius_add_endpoint_by_val(&instance, "GET", "/book", NULL, 0, &callback_read_book, NULL);
    ulfius_add_endpoint_by_val(&instance, "PUT", "/book", NULL, 0, &callback_update_book, NULL);
    ulfius_add_endpoint_by_val(&instance, "DELETE", "/book", NULL, 0, &callback_delete_book, NULL);

    if (ulfius_start_framework(&instance) == U_OK)
    {
        printf("Start framework on port %d\n", instance.port);
        getchar(); // Wait for user input to terminate the program
    }
    else
    {
        fprintf(stderr, "Error starting framework\n");
    }

    ulfius_stop_framework(&instance);
    ulfius_clean_instance(&instance);

    return 0;
}

// Callback to create a new book
int callback_create_book(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    const char *name = json_string_value(json_object_get(json_request, "name"));

    if (name)
    {
        Book newBook;
        newBook.id = 0; // Find the correct ID based on your storage logic
        strncpy(newBook.name, name, sizeof(newBook.name) - 1);

        add_book_to_file(newBook);

        ulfius_set_json_body_response(response, 201, json_pack("{s:i, s:s}", "id", newBook.id, "name", newBook.name));
    }
    else
    {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    json_decref(json_request);

    log_request(request, response);
    return U_CALLBACK_CONTINUE;
}

// Callback to read all books
int callback_read_book(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    int count;
    Book *books = read_all_books_from_file(&count);
    json_t *json_response = json_array();

    for (int i = 0; i < count; i++)
    {
        json_array_append_new(json_response, json_pack("{s:i, s:s}", "id", books[i].id, "name", books[i].name));
    }

    ulfius_set_json_body_response(response, 200, json_response);

    free(books);
    json_decref(json_response);

    log_request(request, response);
    return U_CALLBACK_CONTINUE;
}

// Callback to update a book
int callback_update_book(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int id = json_integer_value(json_object_get(json_request, "id"));
    const char *newName = json_string_value(json_object_get(json_request, "name"));

    if (newName)
    {
        update_book_in_file(id, newName);
        ulfius_set_json_body_response(response, 200, json_pack("{s:i, s:s}", "id", id, "name", newName));
    }
    else
    {
        ulfius_set_string_body_response(response, 400, "Invalid data");
    }

    json_decref(json_request);

    log_request(request, response);
    return U_CALLBACK_CONTINUE;
}

// Callback to delete a book
int callback_delete_book(const struct _u_request *request, struct _u_response *response, void *user_data)
{
    json_t *json_request = ulfius_get_json_body_request(request, NULL);
    int id = json_integer_value(json_object_get(json_request, "id"));

    delete_book_from_file(id);

    json_t *empty_json = json_object();
    ulfius_set_json_body_response(response, 204, json_object());

    json_decref(json_request);
    json_decref(empty_json);
    log_request(request, response);
    return U_CALLBACK_CONTINUE;
}

// Adds a book to the file
void add_book_to_file(Book book)
{
    FILE *file = fopen(FILE_NAME, "ab+");
    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        int id = (fileSize / sizeof(Book)) ? (fileSize / sizeof(Book)) : 0;
        book.id = id;
        fwrite(&book, sizeof(Book), 1, file);
        fclose(file);
    }
    else
    {
        printf("Failed to open file\n");
    }
}

// Reads all books from the file
Book *read_all_books_from_file(int *count)
{
    FILE *file = fopen(FILE_NAME, "rb");
    if (file == NULL)
    {
        *count = 0;
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    *count = fileSize / sizeof(Book);
    Book *books = malloc(fileSize);

    fread(books, sizeof(Book), *count, file);
    fclose(file);

    return books;
}

// Updates a book in the file
void update_book_in_file(int id, const char *newName)
{
    int count;
    Book *books = read_all_books_from_file(&count);

    for (int i = 0; i < count; i++)
    {
        if (books[i].id == id)
        {
            strcpy(books[i].name, newName);
            break;
        }
    }

    FILE *file = fopen(FILE_NAME, "wb");
    fwrite(books, sizeof(Book), count, file);
    fclose(file);
    free(books);
}

// Deletes a book from the file
void delete_book_from_file(int id)
{
    int count, indexToDelete = -1;
    Book *books = read_all_books_from_file(&count);

    for (int i = 0; i < count; i++)
    {
        if (books[i].id == id)
        {
            indexToDelete = i;
            break;
        }
    }

    if (indexToDelete != -1)
    {
        for (int i = indexToDelete; i < count - 1; i++)
        {
            books[i] = books[i + 1];
        }
        count--; // Reduce the count to reflect the deletion
    }

    FILE *file = fopen(FILE_NAME, "wb");
    fwrite(books, sizeof(Book), count, file);
    fclose(file);
    free(books);
}
