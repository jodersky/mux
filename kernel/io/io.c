#include "io/io.h"

int open(struct file* file) {
    file->fops->open(file);
}

int ioctl(struct file* file, int cmd, long args) {
    file->fops->ioctl(file, cmd, args);
}

ssize_t read(struct file* file, char* const buffer, size_t size) {
    struct file_operations* fops = file->fops;
    if (fops->read == NULL) return -1;
    else fops->read(file, buffer, size);
}

ssize_t write(struct file* file, const char* const buffer, size_t size) {
    struct file_operations* fops = file->fops;
    if (fops->write == NULL) return -1;
    else fops->write(file, buffer, size);
}

int close(struct file* file) {
    file->fops->close(file);
}