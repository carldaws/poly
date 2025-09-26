#!/bin/bash

# Generate embedded bundles header file
output="src/embedded_bundles.h"

cat > "$output" << 'EOF'
#ifndef EMBEDDED_BUNDLES_H
#define EMBEDDED_BUNDLES_H

typedef struct {
    const char *name;
    const char *content;
} Bundle;

EOF

echo "static const Bundle embedded_bundles[] = {" >> "$output"

for lua_file in src/bundles/*.lua; do
    if [ -f "$lua_file" ]; then
        basename=$(basename "$lua_file" .lua)
        echo "    {\"$basename\", " >> "$output"

        # Convert file to C string literal
        echo -n "        \"" >> "$output"
        # Use awk to handle the last line properly
        awk '{
            gsub(/\\/, "\\\\");
            gsub(/"/, "\\\"");
            gsub(/\t/, "\\t");
            printf "%s\\n", $0
        }' "$lua_file" >> "$output"
        echo -n "\"" >> "$output"
        echo "}," >> "$output"
    fi
done

cat >> "$output" << 'EOF'
    {NULL, NULL}
};

#endif
EOF

echo "Generated $output"