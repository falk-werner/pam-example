#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>
#include <getopt.h>
#include <termios.h>

#include <security/pam_appl.h>

#define MAX_STRING_LENGTH 80

#define ON (true)
#define OFF (false)

static char * read_line(void)
{
    char * result = NULL;

    char buffer[MAX_STRING_LENGTH];
    char * line = fgets(buffer, MAX_STRING_LENGTH, stdin);
    if (line == buffer)
    {
        size_t length = strlen(line);
        if ((0 < length) && (line[length - 1] == '\n'))
        {
            line[length - 1] = '\0';
        }

        result = strdup(line);
    }

    return result;
}

static bool set_echo(bool value)
{
    bool old_value = ON;

    struct termios attributes;
    struct termios new_attributes;
    int rc = tcgetattr(STDIN_FILENO, &attributes);
    if (0 == rc) 
    {
        new_attributes = attributes;
        old_value = (0 != (new_attributes.c_lflag & ECHO));

        if (ON == value)
        {
            new_attributes.c_lflag |= ECHO;
        }
        else
        {
            new_attributes.c_lflag &= ~ECHO;
        }
        
        tcsetattr(STDIN_FILENO, TCSANOW, &new_attributes);
    }

    return old_value;
}

static char * read_secret()
{
    bool echo_mode = set_echo(OFF);
    char * result = read_line();
    puts("");
    set_echo(echo_mode);

    return result;
}

static int custom_conversation(
    int count,
    struct pam_message const ** messages,
    struct pam_response * * responses,
    void * user)
{
    (void) user;
    *responses = malloc(count * sizeof(struct pam_response));

    for (int i = 0; i < count; i++)
    {
        struct pam_response * response = responses[i];           
        response->resp_retcode = 0;
        response->resp = NULL;

        int message_type = messages[i]->msg_style;
        char const * message = messages[i]->msg;
        switch (message_type)
        {
            case PAM_PROMPT_ECHO_OFF:
                printf("%s", message);
                response->resp = read_secret(); 
                break;
            case PAM_PROMPT_ECHO_ON:
                printf("%s", message);
                response->resp = read_line(); 
                break;
            case PAM_ERROR_MSG:
                fprintf(stderr, "error: %s\n ", message);
                break;
            case PAM_TEXT_INFO:
                printf("info: %s\n", message);
                break;
            default:
                fprintf(stderr, "error: unknown pam message type (%d)\n", message_type);
                break;
        }
    }

    return PAM_SUCCESS;
}

int main(int argc, char * argv[])
{
    int rc;
    pam_handle_t * handle = NULL;
    struct pam_conv conversation =
    {
        &custom_conversation,
        NULL
    };

    printf("authentication service: ");
    char * service_name = read_line();

    rc = pam_start(service_name, NULL, &conversation, &handle);
    if (PAM_SUCCESS != rc)
    {
        fprintf(stderr, "error: failed to initialize pam: %s\n", pam_strerror(handle, rc));
        return EXIT_FAILURE;
    }

    rc = pam_authenticate(handle, 0);
    if (PAM_SUCCESS != rc)
    {
        fprintf(stderr, "error: failed to authenticate user: %s\n", pam_strerror(handle, rc));
        pam_end(handle, 0);
        return EXIT_FAILURE;
    }


    rc = pam_acct_mgmt(handle, 0);
    if (PAM_SUCCESS != rc)
    {
        fprintf(stderr, "error: failed to authenticate user (acct_mgmt): %s\n", pam_strerror(handle, rc));
        pam_end(handle, 0);
        return EXIT_FAILURE;
    }

    printf("SUCCESS\n");

    pam_end(handle, 0);
    return EXIT_SUCCESS;
}