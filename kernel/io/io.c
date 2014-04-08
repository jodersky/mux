#include "mux/io.h"

int open(struct file* file) {
    return file->fops->open(file);
}

int ioctl(struct file* file, int cmd, long args) {
    return file->fops->ioctl(file, cmd, args);
}

ssize_t read(struct file* file, char* const buffer, size_t size) {
    struct file_operations* fops = file->fops;
    if (fops->read == NULL) return -1;
    else return fops->read(file, buffer, size);
}

ssize_t write(struct file* file, const char* const buffer, size_t size) {
    struct file_operations* fops = file->fops;
    if (fops->write == NULL) return -1;
    else return fops->write(file, buffer, size);
}

int close(struct file* file) {
    return file->fops->close(file);
}