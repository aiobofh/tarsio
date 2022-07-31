#!/usr/bin/env bash
for i in $(grep -R '  Copyright (C) 2020 AiO Secure Teletronics' | cut -d':' -f1 | sort -u | grep -v 'update_copyright'); do
    sed -i 's/  Copyright (C) 2020 AiO Secure Teletronics/Copyright (C) 2020-2022 AiO Secure Teletronics/g' $i
done
