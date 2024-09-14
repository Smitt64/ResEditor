function pause() {
    read -s -n 1 -p "Press any key to continue . . ."
    echo ""
}

git submodule update --remote
pause