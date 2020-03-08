dd if=/dev/zero of=sdimage bs=1024 count=1024 # create sdimage file
mke2fs -b 1024 sdimage 1024 # format it as 1mb ext2 fs
sudo mount -o loop sdimage /mnt
sudo mkdir /mnt/bin
sudo umount /mnt