int strcmp(const char* a, const char* b)
{
    while (*a && *b)
    {
        if (*a != *b) return 1;
        a++; b++;
    }
    return (*a != *b);
}

int strncmp(const char* a, const char* b, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i]) return 1;
        if (a[i] == '\0') return 0;
    }
    return 0;
}
