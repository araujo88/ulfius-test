import requests

# Base URL of your C application
BASE_URL = 'http://localhost:8080'

# Test Creating a Book
def create_book(name):
    response = requests.post(f'{BASE_URL}/book', json={'name': name})
    print('Create Book:', response.json())

# Test Reading all Books
def read_books():
    response = requests.get(f'{BASE_URL}/book')
    print('Read Books:', response.json())

# Test Updating a Book
def update_book(id, new_name):
    response = requests.put(f'{BASE_URL}/book', json={'id': id, 'name': new_name})
    print('Update Book:', response.json())

# Test Deleting a Book
def delete_book(id):
    response = requests.delete(f'{BASE_URL}/book', json={'id': id})
    if response.status_code == 204:
        print('Delete Book: No Content')
    else:
        # Attempt to parse the JSON only if the response is not 204
        try:
            print('Delete Book:', response.json())
        except requests.exceptions.JSONDecodeError:
            print('Delete Book: Failed to decode JSON')

# Example usage
if __name__ == '__main__':
    create_book('The Great Gatsby')
    read_books()
    update_book(0, 'The Great Gatsby Updated')
    read_books()
    delete_book(0)
    read_books()
