#ifndef IO_H
#define IO_H

#include <stddef.h>

typedef long ssize_t;

struct file;
struct file_operations;

struct file {
    struct file_operations* const fops;
    void* private_data;
};

struct file_operations {
    int (*open)(struct file* file);
    int (*ioctl)(struct file* file, int cmd, long args);
    ssize_t (*read)(struct file* file, char* const buffer, size_t size);
    ssize_t (*write)(struct file* file, const char* const buffer, size_t size);
    int (*close)(struct file* file);
};

int open(struct file* file);
int ioctl(struct file* file, int cmd, long args);
ssize_t read(struct file* file, char* const buffer, size_t size);
ssize_t write(struct file* file, const char* const buffer, size_t size);
int close(struct file* file);

#endif