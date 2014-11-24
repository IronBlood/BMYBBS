#! /bin/sh

BBS_HOME=/home/bbs
BBSUID=999
BBSGRP=999
TARGET=/home/bbs
HTMPATH=/home/apache/htdocs/bbs
CGIPATH=/home/apache/cgi-bin/bbs
LOCALIP=

# 创建目录

echo -e "\033[1;32m本脚本会在 ${BBS_HOME} 下创建基本目录结构，以及其他需要的文件夹和配置文件"
echo -n "Press <Enter> to continue ..."
read ans

if [ -d ${BBS_HOME} ] ; then
		echo -e "\033[1;31m警告：${BBS_HOME} 已存在！\033[0m"
        echo -n "覆盖 [N]?"
        read ans
        ans=${ans:-N}
        case $ans in
            [Yy]) echo "创建目录到 ${BBS_HOME}" ;;
            *) echo "放弃 ..." ; exit ;;
        esac
else
        echo -e "\033[1;32mMaking dir ${BBS_HOME}\033[0m"
        mkdir ${BBS_HOME}
        chmod -R 770 ${BBS_HOME}
fi

echo -e "\033[1;32mSetup bbs directory tree .....\033[0m"

(cd ${BBS_HOME};mkdir vote tmp reclog bm bin traced newtrace)
(cd ${BBS_HOME};for i in home mail;do for j in `perl -e "print join(' ',A..Z)"`;do mkdir -p $i/$j;done;done)
(cd ${BBS_HOME};for i in bbslists sysop syssecurity notepad newcomers junk vote .tmp .backnumbers;do mkdir -p boards/$i;done)
(cd ${BBS_HOME};for i in sysop bbslists vote newcomers;do mkdir -p 0Announce/groups/GROUP_0/$i;done)
(cd ${BBS_HOME};mkdir -p 0Announce/groups/GROUP_7/notepad)

( cd bbshome ; tar cf - * ) | ( cd ${BBS_HOME} ; tar xf - )
mv ${BBS_HOME}/BOARDS ${BBS_HOME}/.BOARDS
mv ${BBS_HOME}/badname ${BBS_HOME}/.badname
mv ${BBS_HOME}/bad_email ${BBS_HOME}/.bad_email
touch ${BBS_HOME}/.hushlogin

mkdir ${BBS_HOME}/bbstmpfs
ln ${BBS_HOME}/bbstmpfs/tmp ${BBS_HOME}/tmpfast -s
ln ${BBS_HOME}/bbstmpfs/dynamic ${BBS_HOME}/dynamic -s
( cd ${BBS_HOME}/0Announce/bbslist/; for i in day month year week; do ln -s ${BBS_HOME}/etc/posts/$i $i; done )

mkdir -p ${CGIPATH}
mkdir -p ${HTMPATH}

cat > ${BBS_HOME}/etc/sysconf.ini << EOF
#---------------------------------------------------------------
# Here is where you adjust the BBS System Configuration
# Delete ../sysconf.img to make the change after modification
#---------------------------------------------------------------

BBSHOME         = "/home/bbs"
BBSID           = "BMY"
BBSNAME         = "����ٸBBS"
BBSDOMAIN       = "${LOCALIP}"
BBSIP           = "${LOCALIP}"

KEEP_DELETED_HEADER     = 0

#---------------------------------------------------------------
# EMAILFILE  - Toggle the E-Mail Registration Feature
# NEWREGFILE - Toggle the 3 days no-post feature for new comers
#---------------------------------------------------------------
EMAILFILE       = "etc/mailcheck"
#NEWREGFILE     = "etc/newregister"

#---------------------------------------------------------------
# Do not modify anything below unless you know what you are doing...
#---------------------------------------------------------------
PERM_BASIC      = 0x00001
PERM_CHAT       = 0x00002
PERM_PAGE       = 0x00004
PERM_POST       = 0x00008
PERM_LOGINOK    = 0x00010
PERM_DENYSIG    = 0x00020
PERM_CLOAK      = 0x00040
PERM_SEECLOAK   = 0x00080
PERM_XEMPT      = 0x00100
PERM_WELCOME    = 0x00200
PERM_BOARDS     = 0x00400
PERM_ACCOUNTS   = 0x00800
PERM_CHATCLOAK  = 0x01000
PERM_OVOTE      = 0x02000
PERM_SYSOP      = 0x04000
PERM_POSTMASK   = 0x08000
PERM_ANNOUNCE   = 0x10000
PERM_OBOARDS    = 0x20000
PERM_ACBOARD    = 0x40000
PERM_NOZAP      = 0x80000
PERM_FORCEPAGE  = 0x100000
PERM_EXT_IDLE   = 0x200000
PERM_SPECIAL1   = 0x400000
PERM_SPECIAL2   = 0x800000
PERM_SPECIAL3   = 0x1000000
PERM_SPECIAL4   = 0x2000000
PERM_SPECIAL5   = 0x4000000
PERM_SPECIAL6   = 0x8000000
PERM_SPECIAL7   = 0x10000000
PERM_SPECIAL8   = 0x20000000

AUTOSET_PERM	= PERM_CHAT, PERM_PAGE, PERM_POST, PERM_LOGINOK

PERM_ESYSFILE  =  PERM_SYSOP,PERM_WELCOME,PERM_ACBOARD
PERM_ADMINMENU =  PERM_ACCOUNTS,PERM_OVOTE,PERM_SYSOP,PERM_OBOARDS,PERM_WELCOME,PERM_ACBOARD
PERM_BLEVELS   =  PERM_SYSOP,PERM_OBOARDS
PERM_UCLEAN    =  PERM_SYSOP,PERM_ACCOUNTS
PERM_MAILALL   =  PERM_SYSOP,PERM_SPECIAL4
#include "etc/menu.ini"
EOF

# 创建 bbs-start 脚本
cat > ${BBS_HOME}/bbs-start.sh << EOF
#!/bin/sh

mount tmpfs /home/bbs/bbstmpfs -t tmpfs -o size=32M
for i in brc tmp dynamic userattach; do mkdir /home/bbs/bbstmpfs/\$i; done
chown bbs:bbs /home/bbs/bbstmpfs -R

/home/bbs/bin/bbsd
EOF
chmod +x ${BBS_HOME}/bbs-start.sh

# 创建 bbs-stop 脚本
cat > ${BBS_HOME}/bbs-stop.sh << EOF
#!/bin/sh

killall bbsd
umount ${BBS_HOME}/bbstmpfs
EOF
chmod +x ${BBS_HOME}/bbs-stop.sh

# 创建 apache2 配置文件
if [ -f /etc/apache2/sites-available ]
then
cat > /etc/apache2/sites-available/bbs << EOF
NameVirtualHost ${LOCALIP}:80
<VirtualHost ${LOCALIP}:80>
        ServerName ${LOCALIP}:80
        ServerAdmin program_team@bmy
        DocumentRoot ${HTMPATH}
        AssignUserID bbs bbs
        <Directory />
                Options FollowSymLinks
                AllowOverride None
        </Directory>
        <Directory ${HTMPATH}>
                Options Indexes FollowSymLinks MultiViews
                AllowOverride None
                Order allow,deny
                allow from all
        </Directory>
        ScriptAlias /cgi-bin/ ${CGIPATH%%/bbs}
        <Directory ${CGIPATH%%/bbs}>
                AllowOverride None
                Options None
                Options +ExecCGI -MultiViews +SymLinksIfOwnerMatch
                Order allow,deny
                Allow from all
        </Directory>

        RewriteEngine on
        RewriteRule ^/BMY(.*)$ /cgi-bin/bbs/www [PT]
        RewriteRule ^/$ /cgi-bin/bbs/www [PT]

        ErrorLog /var/log/apache2/error.log
        LogLevel warn
</VirtualHost>
EOF
a2ensite bbs
fi

# 修改文件夹权限
chown -R ${BBSUID}.${BBSGRP} ${BBS_HOME}
chown -R ${BBSUID}.${BBSGRP} ${CGIPATH}
chown -R ${BBSUID}.${BBSGRP} ${HTMPATH}


echo -e "\033[1;32m配置结束....\033[0m"
echo -e "Check the configuration in \033[1;31m${BBS_HOME}/etc/sysconf.ini\033[0m"
echo -e "现在可以执行编译过程，详细步骤请参考\033[1;31m《BMYBBS权威安装文档》\033[0m"
