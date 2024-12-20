#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/timekeeping.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/rtc.h>

#define PROCFS_NAME "tsu"

static struct proc_dir_entry *our_proc_file = NULL;

static ssize_t procfile_read(struct file *file_pointer, char __user *buffer,
                             size_t buffer_length, loff_t *offset) {
    char s[256];
    size_t len;
    struct timespec64 now;
    struct tm tm_now;
    struct rtc_time rtc_new_year;
    time64_t seconds_to_new_year;

    // Получаем текущее время
    ktime_get_real_ts64(&now);
    time64_to_tm(now.tv_sec, 0, &tm_now);


    memset(&rtc_new_year, 0, sizeof(struct rtc_time));
    rtc_new_year.tm_year = tm_now.tm_year + 1;
    rtc_new_year.tm_mon = 0; 
    rtc_new_year.tm_mday = 1;  
   
    time64_t new_year_seconds = rtc_tm_to_time64(&rtc_new_year);


    seconds_to_new_year = new_year_seconds - now.tv_sec;

    // Если Новый год уже наступил, считаем до следующего нового года
    if (seconds_to_new_year < 0) {
        rtc_new_year.tm_year++;
        new_year_seconds = rtc_tm_to_time64(&rtc_new_year);
        seconds_to_new_year = new_year_seconds - now.tv_sec;
    }

    long days = seconds_to_new_year / (60 * 60 * 24);
    long hours = (seconds_to_new_year % (60 * 60 * 24)) / (60 * 60);
    long minutes = (seconds_to_new_year % (60 * 60)) / 60;
    long seconds = seconds_to_new_year % 60;

    len = snprintf(s, sizeof(s), "Happy New Year!!!\nTime until New Year: %ld days, %ld hours, %ld minutes, %ld seconds\n",
                   days, hours, minutes, seconds);

    if (*offset >= len)
        return 0;

    if (buffer_length == 0)
      return -EINVAL;

    if (buffer_length < len)
        return -EINVAL;

    if (copy_to_user(buffer, s, len)) {
        return -EFAULT;
    }

    *offset += len;

    pr_info("procfile_read: /proc/%s read\n", PROCFS_NAME);
    return len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_fops = {
    .proc_read = procfile_read,
};
#else
static const struct file_operations proc_file_fops = {
    .read = procfile_read,
};
#endif

static int __init procfs1_init(void) {
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops);
    if (our_proc_file == NULL) {
        pr_err("Error: Could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

// Функция очистки модуля
static void __exit procfs1_exit(void) {
    proc_remove(our_proc_file);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(procfs1_init);
module_exit(procfs1_exit);

MODULE_LICENSE("GPL");
