#include <stdio.h>
#include <stdlib.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>

#define MAX_STRING_LENGTH 80

int main(int argc, char * argv)
{
    int rc;
    char service_name[MAX_STRING_LENGTH];   
    pam_handle_t * handle = NULL;
    struct pam_conv conversation =
    {
        misc_conv,
        NULL
    };

    printf("authentication service: ");
    scanf("%s", service_name);

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

    printf("SUCCESS");

    pam_end(handle, 0);
    return EXIT_SUCCESS;
}