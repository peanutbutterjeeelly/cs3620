# include <stdio.h> 
# include <string.h>
# include <openssl/sha.h>
# include <openssl/hmac.h>
# include <openssl/evp.h>
# include <openssl/rand.h>
# include <assert.h> 
# include "util.h" 

//# define DEBUG

char pFile[1024] ; 

void dump_to_password_file(char *fileName)
{
	//printf("DEBUG: password file %s\n",fileName); 
	FILE * fp = fopen(fileName, "w");
	if(!fp){
		printf("Error: Cannot open password file to write\n"); 
		exit(0); 
	}
	dump_datastructure_into_file(fp) ; 
	fclose(fp); 
}

void print_console_message()
{
	
	printf("\n\n\nSelect an integer from 1-5\n"); 
	printf("The integer option maps to the following commands:\n(1) register a user account\n(2) delete a user account\n(3) user login\n(4) change an account password\n(5) quit console\n\n\n");
}

int register_user(unsigned char *user, unsigned char *password){
	char salt[32]; 
	char * mdString;
	unsigned char * binary;	

	printf("Registering user %s\n",user); 
	if (username_okay(user)!=OKAY || password_okay(password)!=OKAY)		
		return ERROR;

	if (find_user(user) != NULL) {
		printf("Error: User already exists\n");
		return ERROR;
	}

	unsigned char digest[SHA512_DIGEST_LENGTH]; 
	SHA512_CTX ctx;
	SHA512_Init(&ctx);
	SHA512_Update(&ctx, password, strlen(password));
	assert(getRandBytes(salt, 32) != -1 ) ; 
	SHA512_Update(&ctx, salt, 32);
	SHA512_Final(digest, &ctx);

	mdString = malloc((SHA512_DIGEST_LENGTH*2 + 1)*sizeof(char));
	unsigned char * binarymdString;
	binary = malloc((SHA512_DIGEST_LENGTH*2 + 1)*sizeof(char));

	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	    sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

	#ifdef DEBUG
	printf("SHA512 hash value: %s\n", mdString);
	printf("LEN: %d\n",SHA512_DIGEST_LENGTH); 
	#endif
	
	hex_array_to_byte_array(mdString,binary);
	push_back(user, salt, binary);

	#ifdef DEBUG        
	printf("DEBUG: Called register_user function\n");
	#endif        
	return OKAY ;
}

int match_user(unsigned char *user, unsigned char * password){
	LLEntry * userNode;
	char * mdString;
	unsigned char * binary;

	if (username_okay(user)!=OKAY || password_okay(password)!=OKAY)		
		return ERROR;

	userNode = find_user(user);
	if (userNode == NULL) {
		printf("Error: User does not exist\n");
		return ERROR;
	}

	unsigned char digest[SHA512_DIGEST_LENGTH]; 
	SHA512_CTX ctx;
	SHA512_Init(&ctx);
	SHA512_Update(&ctx, password, strlen(password));
	SHA512_Update(&ctx, userNode->salt, 32);
	SHA512_Final(digest, &ctx);

	mdString = malloc((SHA512_DIGEST_LENGTH*2 + 1)*sizeof(char));
	binary = malloc((SHA512_DIGEST_LENGTH*2 + 1)*sizeof(char));

	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	    sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

	hex_array_to_byte_array(mdString, binary);

	#ifdef DEBUG
	printf("\nnon-binary: ");
	for(int i=0;i<SHA512_DIGEST_LENGTH;i++) printf("%x ",mdString[i]);
	printf("\nbinary: ");	
	for(int i=0;i<HASHED_PASSWORD_SIZE;i++) printf("%x ",binary[i]);
	printf("\n");	
	for(int i=0;i<HASHED_PASSWORD_SIZE;i++) printf("%x ",userNode->hashed_password[i]);
	#endif

	if (binary_compare(binary, HASHED_PASSWORD_SIZE, userNode->hashed_password, HASHED_PASSWORD_SIZE)==-1) {
		printf("Error: User password does not match\n");
		return ERROR;
	}

        #ifdef DEBUG
	printf("DEBUG: Called login function\n");
	#endif
        return OKAY ;
}

int delete_user(unsigned char *user, unsigned char * password){
	LLEntry * userNode;
	char * mdString;
	unsigned char * binarymdString;

	if (username_okay(user)!=OKAY || password_okay(password)!=OKAY)		
		return ERROR;
 
	userNode = find_user(user);	
	if (user == NULL) {
		printf("Error: User does not exist\n");
		return ERROR;
	}	

	if (match_user(user, password)==ERROR)
		return ERROR;

	delete_node (user);

	#ifdef DEBUG        
	printf("DEBUG: Called delete_user function\n");
	#endif
        return OKAY ;
}

int change_user_password(unsigned char *user, unsigned char * password_current, unsigned char * password_new){
	char pass[1024];
	char salt[32];
	char * mdString;
	unsigned char * binarymdString;
        
	if (username_okay(user)!=OKAY || password_okay(password_current)!=OKAY)		
		return ERROR;

	if (find_user(user) == NULL) {
		printf("Error: User does not exist\n");
		return ERROR;
	}

	if (match_user(user, password_current)==ERROR)
		return ERROR;

	unsigned char digest[SHA512_DIGEST_LENGTH]; 
	SHA512_CTX ctx;
	SHA512_Init(&ctx);
	SHA512_Update(&ctx, password_new, strlen(password_new));
	assert(getRandBytes(salt, 32) != -1 ) ; 
	SHA512_Update(&ctx, salt, 32);
	SHA512_Final(digest, &ctx);

	mdString = malloc((SHA512_DIGEST_LENGTH*2 + 1)*sizeof(char));
	binarymdString = malloc((SHA512_DIGEST_LENGTH*2 + 1)*sizeof(char));

	for (int i = 0; i < SHA512_DIGEST_LENGTH; i++)
	    sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);
	
	hex_array_to_byte_array(mdString, binarymdString);
	update_user_password (user, salt, binarymdString);

        #ifdef DEBUG
	printf("DEBUG: Called change_user_password function\n");
	#endif
        return OKAY ;
}

void process_option(int option){
	switch(option){
		case 1:{
			printf("SELECTED OPERATION: <REGISTER USER ACCOUNT>\n");
			//registering a user account --- 
			printf("Enter username: ");
		    unsigned char username[1024];
		    scanf("%s",username) ;
		    printf("Enter password: ");
		    unsigned char password[1024] ;
		    scanf("%s",password) ;			
			if(register_user(username, password) == OKAY){
				printf("STATUS: Writing to password file\n"); 
				dump_to_password_file( pFile) ; 
				printf("STATUS: Finished writing to password file\n"); 
			}
			else{
				printf("Error: Cannot register user\n");
				
			}
			break ; 
		}
		
		case 2:{
			printf("SELECTED OPERATION: <DELETE USER ACCOUNT>\n");
			printf("Enter username to delete: ");
		    unsigned char username[1024];
		    scanf("%s",username) ;

		    printf("Enter password: ");
		    unsigned char password[1024];
		    scanf("%s",password) ;
			if(delete_user(username,password)==OKAY){
				printf("User deleted\n");
				printf("STATUS: Writing to password file\n"); 
				dump_to_password_file( pFile) ; 
				printf("STATUS: Finished writing to password file\n"); 

			}
			else{
				printf("Error: Cannot delete user\n"); 
			}

			break ; 
		} 

		case 3:{
			printf("SELECTED OPERATION: <USER LOGIN>\n");
			printf("Enter username: ");
		    unsigned char username[1024];
		    scanf("%s",username) ;
		    printf("Enter password: ");
		    unsigned char password[1024] ;
		    scanf("%s",password) ;
			if(match_user(username,password)==OKAY){
				printf("________________________\n");
				printf("|                       |\n");
				printf("| USER LOGIN SUCCESSFUL |\n");
				printf("|_______________________|\n");
			}
			else{ 
				printf("__________________________\n");
				printf("|                         |\n"); 
				printf("| USER LOGIN UNSUCCESSFUL |\n");
				printf("|_________________________|\n");
			}
			break ; 
		} 

		case 4: {
			printf("SELECTED OPERATION: <CHANGE PASSWORD>\n");
			printf("Enter username: ");
		    unsigned char username[1024];
		    scanf("%s",username) ;
		    printf("Enter current password: ");
		    unsigned char password[1024] ;
		    scanf("%s",password) ;
		    printf("Enter new password: ");
		    unsigned char npassword[1024] ;
		    scanf("%s",npassword) ;
			if(strcmp(password,npassword)==0){
				printf("Error: New and Old password are same\n");
				break ; 
			}
			if(change_user_password(username,password,npassword) == OKAY){
				printf("User password changed\n");
				printf("STATUS: Writing to password file\n"); 
				dump_to_password_file( pFile) ; 
				printf("STATUS: Finished writing to password file\n"); 
				
			}
			else printf("Error: Cannot change user password\n"); 
			break ; 
		}

		case 5: {
			printf("Error: Option 5 should not be handled here\n"); assert(0); break ; 
		}

		default: {
			printf("Error: Unknown option %d\n",option); assert(0); break ; 
		}

	}

}



int main(int argc, char *argv[]){

	if(argc != 2){
		printf("Error: Missing Arguments\n");
		printf("Usage: ./Executable-Name Password-File-Name\n"); 
		return 0 ; 
	}

	FILE * fp = fopen(argv[1], "r"); 
	if(!fp){
		printf("Error: Cannot open input file %s\n",argv[1]); 
		return 0 ; 
	}
	//load the password file in a data-structure 	
	parse_line_load_data_structure(fp) ; 
	fclose(fp) ; 

	strcpy(pFile, argv[1]); 

	while(1)
	{
		print_console_message(); 
		int option ; 
		printf("OPTION: "); 
		scanf("%d",&option);
		if(option==5)break; 
		process_option(option); 
		//dump_structure();
	}
	dump_to_password_file( pFile) ;

	//clean up data structure; 
	delete_entire_list() ; 

	return 0 ; 
}

