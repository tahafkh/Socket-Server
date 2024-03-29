#define CONFIG_FILE_PATH  "includes/config.json"

#define USER_COMMAND "user"
#define PASS_COMMAND "pass"
#define RETR_COMMAND "retr"
#define UPLOAD_COMMAND "Upload"
#define HELP_COMMAND "help"
#define QUIT_COMMAND "quit"


#define SPACE " "
#define COLON ": "
#define EMPTY ""
#define SLASH "/"
#define LOG_FILE "log.txt"

#define DELETE_CODE "250"
#define CREATE_CODE "257"

#define DELE_DIRECTORY "-d"
#define DELE_FILE "-f"
#define INFO "214\n"
#define USERNAME_ACCEPTED "331: User name Okay, need password. "
#define BAD_SEQUENCE "503: Bad sequence of commands. "
#define SUCCESSFUL_LOGIN "230: User looged in, proceed. Logged out if appropriate. "
#define INVALID_USER_PASS "430: Invalid username or password "
#define FILE_UNAVAILABLE "550: File unavailable. "
#define LIST_TRANSFER_DONE "226: List transfer done. "
#define SUCCESSFUL_CHANGE "250: Successful change. "
#define SUCCESSFUL_DOWNLOAD "226: Successful Download. "
#define SUCCESSFUL_QUIT "221: Successful Quit. "
#define NEED_ACCOUNT "332: Need account for login. "
#define SYNTAX_ERROR "501: Syntax error in parameters or arguments. "
#define ERROR "500: Error "
#define DOWNLOAD_LIMIT_SIZE "‫‪425:‬‬ ‫‪Can't‬‬ ‫‪open‬‬ ‫‪data‬‬ ‫‪connection.‬‬ "

#define USER_DESCRIPTION "user [name], Its argument is used to specify the user's string. It is used for user authentication.\n "
#define PASS_DESCRIPTION "pass [password], Its argument is used to specify the user's password. It is used for user authentication.\n "
#define RETR_DESCRIPTION "retr [name], Its argument is used to specify the file's name. It is used to download a file.\n "
#define UPLOAD_DESCRIPTION "Upload [name] This command loads the given file, which can be in any location, in the program's main directory and in a folder called 'Files'."
#define HELP_DESCRIPTION "help, It is used to display information about builtin commands.\n "
#define QUIT_DESCRIPTION "quit, logout from account.\n "

