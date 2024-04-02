# Ulfius REST API test

This project implements a simple Book Management API using the Ulfius framework in C for the backend service and Python for testing the CRUD operations. The API allows creating, reading, updating, and deleting book records, which are stored in a file.

## Prerequisites

Before you begin, ensure you have met the following requirements:

- GCC compiler for C
- Ulfius Framework for the web server and REST functionality
- Jansson library for JSON handling in C
- Python 3.x and `requests` library for testing

## Installation

### Backend API

1. **Install Ulfius and Jansson Libraries**: Follow the instructions from the official [Ulfius documentation](https://github.com/babelouest/ulfius) and [Jansson documentation](https://digip.org/jansson/) to install these libraries on your system.

2. **Compile the C Application**:
   Navigate to the project directory and compile the code using GCC. Make sure to link the Ulfius and Jansson libraries.
   ```sh
   gcc -o main main.c -lulfius -ljansson
   ```

### Python Tests

1. **Install Python 3.x**: Ensure Python 3.x is installed on your system.

2. **Install Requests Library**:
   Use pip to install the `requests` library.
   ```sh
   pip install requests
   ```

## Usage

To start the Book Management API, run the compiled binary:

```sh
./main
```

The API will start listening for HTTP requests on port 8080.

## Testing

To test the CRUD operations, execute the Python script `tests.py` located in the `tests` directory. Make sure the API is running before executing the tests.

```sh
python tests.py
```

This script tests creating a new book, reading all books, updating a book, and deleting a book.

## Contributing

Contributions to this project are welcome. To contribute:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature/AmazingFeature`).
3. Make your changes and commit them (`git commit -m 'Add some AmazingFeature'`).
4. Push to the branch (`git push origin feature/AmazingFeature`).
5. Open a pull request.

## License

This project is licensed under the LGPL-2.1 License - see the LICENSE file for details.
