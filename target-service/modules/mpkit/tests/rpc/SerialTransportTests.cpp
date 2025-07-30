/*
 * It is pretty difficult to test anything in SerialTransport as every class
 * tries to open a device in its ctor and creating such a device's mock
 * seems difficult under Linux and almost impossible under Windows
 *
 * If someone have an idea about how to do that, you are welcome to write such
 * a tests
 */
