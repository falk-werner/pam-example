#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <security/pam_appl.h>

#define MAX_STRING_LENGTH 80

static int custom_conversation(
    int count,
    struct pam_message const ** messages,
    struct pam_response * * responses,
    void * user)
{
    char const * password = (char const *) user;

    *responses = malloc(count * sizeof(struct pam_response));

    for (int i = 0; i < count; i++)
    {
        struct pam_response * response = responses[i];   

        response->resp_retcode = 0;
        response->resp = malloc(strlen(password) + 1);
        strcpy(response->resp, password);        
    }

    return PAM_SUCCESS;
}

int main(int argc, char * argv[])
{
    if (3 <= argc)
    {    
        int rc;
        char const * service_name = argv[1];   
        char const * user = argv[2];   
        char const * password = argv[3];   
        pam_handle_t * handle = NULL;
        struct pam_conv conversation =
        {
            &custom_conversation,
            (void *) password
        };

        rc = pam_start(service_name, NULL, &conversation, &handle);
        if (PAM_SUCCESS != rc)
        {
            fprintf(stderr, "error: failed to initialize pam: %s\n", pam_strerror(handle, rc));
            return EXIT_FAILURE;
        }

        pam_set_item(handle, PAM_USER, user);        
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

        printf("SUCCESS");

        pam_end(handle, 0);
    }
    else
    {
        printf("usage: ./non-interactive <service> <user> <password>\n");
    }
    
    return EXIT_SUCCESS;
}