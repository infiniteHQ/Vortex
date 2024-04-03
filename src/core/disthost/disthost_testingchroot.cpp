#include "../../../src/vortex.h"
#include "../../../src/vortex_internals.h"

void VxDistHost::TestingChroot(){

  std::string CMD_AddToGroup = "groupadd vortex";
  std::string CMD_AddUser = "useradd -s /bin/bash -g vortex -m -k /dev/null vortex"; // + " -p " + user.vxHostUser_Crypto;
  std::string CMD_CreateUserDirectory = "mkdir -pv /home/vortex";
  std::string CMD_GiveUserDirectoryToUser = "sudo chown -v vortex:vortex  /home/vortex";
  std::string CMD_AddToSudoers = "sudo usermod -aG root vortex";

  system((char *)CMD_AddToGroup.c_str());
  system((char *)CMD_AddUser.c_str());
  system((char *)CMD_CreateUserDirectory.c_str());
  system((char *)CMD_GiveUserDirectoryToUser.c_str());
  system((char *)CMD_AddToSudoers.c_str());

  std::string path = this->path + "/" + this->target + "/";


  std::string CMD_CreateDirectories = "mkdir -pv " + path + "/{dev,proc,sys,run}";
  std::string CMD_Console = "mknod -m 600 " + path + "/dev/console c 5 1";
  std::string CMD_Null = "mknod -m 666 " + path + "/dev/null c 1 3";
  std::string CMD_MountDev = "mount -v --bind /dev " + path + "/dev";
  std::string CMD_MountDevPTS = "mount -v --bind /dev/pts " + path + "/dev/pts";
  std::string CMD_MountProc = "mount -vt proc proc " + path + "/proc";
  std::string CMD_MountSysfs = "mount -vt sysfs sysfs " + path + "/sys";
  std::string CMD_MountTmpFs = "mount -vt tmpfs tmpfs " + path + "/run";

  system((char *)CMD_CreateDirectories.c_str());
  system((char *)CMD_Console.c_str());
  system((char *)CMD_Null.c_str());
  system((char *)CMD_MountDev.c_str());
  system((char *)CMD_MountDevPTS.c_str());
  system((char *)CMD_MountProc.c_str());
  system((char *)CMD_MountSysfs.c_str());
  system((char *)CMD_MountTmpFs.c_str());


  std::string CMD_Simlink = "if [ -h " + path + "/dev/shm ]; then \n mkdir -pv " + path + "/$(readlink " + path + "/dev/shm) else mount -t tmpfs -o nosuid,nodev tmpfs " + path + "/dev/shm \n fi";
  std::cout << CMD_Simlink << std::endl;
  system((char *)CMD_Simlink.c_str());


  std::string CMD_Chroot = "chroot \"" + path + "\" /usr/bin/env -i HOME=/root TERM=\"$TERM\" PS1='(vortex chroot) \\u:\\w\\$ ' PATH=/usr/bin:/usr/sbin:" + path + "\" " + "/bin " + "\" " + "/usr/bin/bash --login";
  
  CMD_Chroot += " -c 'ls && exit'";

  std::cout << CMD_Chroot << std::endl;
  system((char *)CMD_Chroot.c_str());
  system((char *)CMD_Chroot.c_str());
  system((char *)CMD_Chroot.c_str());
  system((char *)CMD_Chroot.c_str());
  system((char *)CMD_Chroot.c_str());
  


  std::string CMD_MountPoint = "mountpoint -q " + path + "/dev/shm && umount " + this->path + "/dev/shm";
  std::string CMD_UnmountDev = "umount " + path + "/dev/pts";
  std::string CMD_Unmount = "umount " + path + "/{sys,proc,run,dev}";

  system((char *)CMD_MountPoint.c_str());
  system((char *)CMD_UnmountDev.c_str());
  system((char *)CMD_Unmount.c_str());

  system("whoami");  
  system("whoami");  
  system("pwd");
  system("pwd");

}
