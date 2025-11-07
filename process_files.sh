#!/usr/bin/env bash

#if keyword exists
if [ -z "$1" ]; then
    echo "Usage: $0 KEYWORD"
    exit 1
fi

KEYWORD="$1"
OUTPUT_FILE="result.log"

#clean
>"$OUTPUT_FILE"

#Searching all .txt files
COUNT=0
while IFS= read -r FILE; do
    # Searching Keywords
    if grep -q "$KEYWORD" "$FILE"; then
        echo "$FILE" >> "$OUTPUT_FILE"     # output to  result.log
        chmod 444 "$FILE"                  # read only
        COUNT=$((COUNT + 1))
    fi
done < <(find . -type f -name "*.txt")

echo "查找到 ${COUNT} 个文件，权限已更改为只读。操作完成！"
