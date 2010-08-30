


void setup_environment() {
    char tname[1024] = "/tmp/gummi_XXXXXXX"; 
    int fh = mkstemp(tname); 
    slog(L_INFO, "Filename %s\n", tname); 
}
