#!/bin/sh
clear
rm test.db
find /boot/develop/headers/os -name "*.h" -exec lua ./IndexHaikuHeader.lua test.db '{}' \;
echo "------------------------"
echo "Files"
echo "------------------------"
sqlite3 test.db "select * from Files;"
echo "------------------------"
echo "Classes"
echo "------------------------"
sqlite3 test.db "select * from Classes;"
echo "------------------------"
echo "Class Entries"
echo "------------------------"
sqlite3 test.db "select * from ClassEntries;"
echo "------------------------"
echo "Defines"
echo "------------------------"
sqlite3 test.db "select * from Defines;"
echo "------------------------"
echo "Enums"
echo "------------------------"
sqlite3 test.db "select * from Enums;"

