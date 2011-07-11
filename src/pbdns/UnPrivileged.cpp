#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "UnPrivileged.hpp"
#include <iostream>

UnPrivileged::UnPrivileged():mDroppedPriv(false),mIsChild(false) {
   struct passwd *nobody=getpwnam("nobody");
   if (nobody) {
      uid_t nobody_uid= nobody->pw_uid;
      gid_t nobody_gid= nobody->pw_gid;
      if (setgid(nobody_gid) == 0) {
        if (setuid(nobody_uid) == 0) {
          mDroppedPriv=true;
          pid_t pid;
          pid = fork();
          if (pid == 0) {
            std::cerr << "Child here." << std::endl;
            setsid();
            close(0);
            close(1);
            close(2);
            int f1=open("/dev/zero",O_RDWR);
            int f2=open("/tmp/pbdns.stdout",O_RDWR|O_CREAT,0700);
            int f3=open("/tmp/pbdns.stderr",O_RDWR|O_CREAT,0700);
            umask(0000);
            chdir("/");
            mIsChild=true;
          }
        }
      }            
   }
}
bool UnPrivileged::operator()() {
  return mDroppedPriv;
}
bool UnPrivileged::is_child(){
  return mIsChild;
}
