#!/bin/bash
if git add . && git commit -m 'update'; then
git push origin main
else
echo -e '\nCould not commit local repo\n'
fi
