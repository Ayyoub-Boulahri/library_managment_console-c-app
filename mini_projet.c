#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <conio.h>

// ****************************************************************************************************
// ***************************************     GLOBALS    *********************************************
// ****************************************************************************************************

#define user_nom_file "users.txt"
#define admins_nom_file "admins.txt"
#define categories_nom_file "categories.txt"
#define livre_nom_file "livres.txt"
#define emprunts_nom_file "emprunts.txt"
#define demandes_nom_file "demmande.txt"
#define auteurs_nom_file "auteurs.txt"
#define demande_refuse_nom_file "demande_refuse.txt"
#define emprunts_online_nom_file "emprunts_online.txt"
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

typedef struct user {
    char *cin;
    char *nom;
    char *prenom;
    char *email;
    char *telephone;
    char *password;
    struct user *suivant;
} user;

typedef struct livre {
    char nom_livre[100];
    int date;
    int id_auteur;
    int id_categorie;
    int id_livre;
    struct livre *gauche;
    struct livre *droit;
}livre;

typedef struct auteur{
    char nom_aut[50];
    char prenom_aut[50];
    int id_aut;
    struct auteur *suivant;
}auteur;

typedef struct categorie {
    int id_categorie;
    char intitule[50];
    struct categorie *suivant;
}categorie;

typedef struct demande{
   char cin[20];
   int  id_livre;
   struct demande *suivant;
}demande;

typedef struct emprunts {
    char cin[20];
    int id_livre;
    char date_debut[11];
    char date_fin[11];
} emprunts;

typedef struct file{
    demande *tete;
    demande *queue;
    int taille;
}file;

void after_user_login_menu(user* user_list,user* u,auteur* aut_list,categorie* cat_list,livre* livre_arbre);
void after_admin_login_menu(user *user_list, auteur* aut_list);
void main_menu(user *user_list,livre* livre_arbre);
void affiche_livre_by_cat(livre *livre_arbre,int id_categorie,auteur *aut_list);
void menu_des_livres(livre *livre_arbre,user *user_list, auteur* aut_list,categorie* cat_list);
void menu_des_categorie(categorie *cat_list,livre* livre_arbre, user* user_list,auteur* aut_list);
void menu_des_emprunts(file* dem_file,user *user_list, auteur *aut_list,livre* livre_arbre);
void menu_users(user *user_list, livre* livre_arbre, auteur* aut_list);
void menu_des_auteurs(auteur *aut_list,livre* livre_arbre,user* user_list);
void supprimer_demande_by_livre(int id_livre);
void supprimer_emprunt_by_livre(int id_livre);
void supprimer_emp_refu_by_livre(int id_livre);
void supprimer_emp_online_by_livre(int id_livre);

// ****************************************************************************************************
// ***********************************    GENERAL FONCTION    *****************************************  
// ****************************************************************************************************

// fonction qui generer le logo du application

void decorate(){
    system("cls");
    printf("\n\n\n");
    printf("\t\t\t\t\t");
    printf("\033[31m");
    printf("%c", 201);
    for (int i = 0; i < 60; i++) {
        printf("%c", 205);
    }
    printf("%c\n", 187); 
    printf("\n\t\t\t\t\t\t\033[1;33m.____    ._____.\n");                             
    printf("\t\t\t\t\t\t|    |   |__\\_ |______________ _______ ___.__.\n");
    printf("\t\t\t\t\t\t|    |   |  || __ \\_  __ \\__  \\\\_  __ <   |  |\n");
    printf("\t\t\t\t\t\t|    |___|  || \\_\\ \\  | \\// __ \\|  | \\/\\___  |\n");
    printf("\t\t\t\t\t\t|_______ \\__||___  /__|  (____  /__|   / ____|\n");
    printf("\t\t\t\t\t\t        \\/       \\/           \\/       \\/     \33[31m\n");


    printf("\t\t\t\t\t%c",200);
    for (int i = 0; i < 60; i++) {
        printf("%c", 205);
    }
    printf("%c",188);
    printf("\033[0m");
    printf("\n\n\n");
}

// progress bar animation fonction

void progressAnimation() {
    system("cls");
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    system("color 0A");
    char a = 177, b = 219;
    printf("\t\t");
    for (int i = 0; i < 88; i++) {
        printf("%c", a);
        printf("\r");
        printf("\t\t");
    }
    for (int i = 0; i < 88; i++) {
        printf("%c", b);
        Sleep(10);
    }
}

// intialiser la file qui gerer les demandes d'emprunts

void init_file_demande(file *f){
    f->tete = NULL;
    f->queue = NULL;
    f->taille = 0;
}

// fonction qui ajoute un element a la file du demandes d'emprunts

int enfiler(file *dem_fil, demande *nv_dem) {
    if (dem_fil->taille == 0)
    {                                    
        dem_fil->queue = nv_dem;      
        dem_fil->tete = nv_dem;       
    }
    else{                                        
        dem_fil->queue->suivant = nv_dem;
        dem_fil->queue = nv_dem;        
    }
    nv_dem->suivant = NULL; 
    dem_fil->taille++;
    return 0;
}

// fonction pour supprimer un elemnt de la file des demandes d'emprunts

int defiler(file *dem_file) {
    if(dem_file->taille == 0)
        return -1;
    demande *supp = dem_file->tete;
    dem_file->tete = dem_file->tete->suivant;
    dem_file->taille--;
    free(supp);
    return 0;
}

// remplir la file des demandes du fichier des demandes

void remplir_file(file *f) {
    FILE *p_file_demande = fopen(demandes_nom_file,"r");
    if(!p_file_demande){
        printf("ERREUR!!!\n");
        return;
    }
    while(!feof(p_file_demande)) {
        demande *nv_dem = (demande*)malloc(sizeof(demande));
        if(fscanf(p_file_demande,"%s\n%d\n",nv_dem->cin,&nv_dem->id_livre) == 2)
            enfiler(f,nv_dem);
    }
    fclose(p_file_demande);
}

// initialiser la list des categories

categorie *init_cat_list(categorie *cat_list) {
    cat_list = NULL;
    return cat_list;
}

// initialiser la list des auteurs

auteur *init_auteurs_list(auteur *aut_list) {
    aut_list = NULL;
    return aut_list;
}

// intitialiser la liste des utilisateurs

user *init_user_list(user *list) {
    list = NULL;
    return list;
}

// fonction pour allouer la memoire pour un utilisateur

user *allocation_dynamique() {
    user *temp = (user*)malloc(sizeof(user));
    if(!temp)
        return NULL;
    temp->cin = (char*)malloc(30*sizeof(char));
    if(!temp->cin)
        return NULL;
    temp->nom = (char*)malloc(30*sizeof(char));
    if(!temp->nom)
        return NULL;
    temp->prenom = (char*)malloc(30*sizeof(char));
    if(!temp->prenom)
        return NULL;
    temp->email = (char*)malloc(30*sizeof(char));
    if(!temp->email)
        return NULL;
    temp->telephone = (char*)malloc(30*sizeof(char));
    if(!temp->telephone)
        return NULL;
    temp->password = (char*)malloc(30*sizeof(char));
    if(!temp->password)
        return NULL;
    return temp;
}

// fonction pour free la memoire allouer par un utilisateur

void free_user_allocation(user *u) {
    free(u->cin);
    free(u->nom);
    free(u->prenom);
    free(u->email);
    free(u->telephone);
    free(u->password);
    free(u);
}

// fonction qui returner la date courrante

char* get_current_date() {
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    static char date[11];
    sprintf(date, "%04d-%02d-%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
    return date;
}

// fonction qui returner la date apre 15 jour depuis là

char* get_current_date_plus_15_days() {
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    local->tm_mday += 15;
    mktime(local);
    static char date[11];
    sprintf(date, "%04d-%02d-%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
    return date;
}


// ****************************************************************************************************
// *************************************    SIGN UP USER    *******************************************  
// ****************************************************************************************************

// fonction pour lire les information d'un utilisateur

void get_info_sign_up(user *u) {
    printf("le cin : ");
    scanf("%s",u->cin);
    printf("le nom : ");
    scanf("%s",u->nom);
    printf("le prenom : ");
    scanf("%s",u->prenom);
    printf("Email : ");
    scanf("%s",u->email);
    printf("le telephone : ");
    scanf("%s",u->telephone);
    printf("Password : ");
    scanf("%s",u->password);
} 

// fonction pour ajouter un utilisateur dans la list des users

user* ajoute_en_fin(user* list, user* nv) {
    nv->suivant = NULL;
    
    // si la liste est vide
    if(list == NULL) { 
        return nv;
    }

    // la liste n'est pas vide on cherche le dernier utilisateur
    user* dernier = list;
    while(dernier->suivant != NULL) {
        dernier = dernier->suivant;
    }

    dernier->suivant = nv;
    return list;
}

// remplir la list des utilisateur a partir du fichier users.txt

user *remplir_user(user *list) {
    FILE *p_file_user = fopen(user_nom_file, "r+");
    if (!p_file_user) { 
        return NULL;
    }

    while (!feof(p_file_user)) {
        user *nv = allocation_dynamique();
        if (!nv) {
            fclose(p_file_user);
            return NULL;
        }
        fscanf(p_file_user, "%s\n%s\n%s\n%s\n%s\n%s\n", nv->cin, nv->nom, nv->prenom, nv->email, nv->telephone, nv->password);
        list = ajoute_en_fin(list, nv);
    }

    fclose(p_file_user);
    return list;
}

// fonction pour verifier si l'utilisateur est deja enregistrer dans l'application par l'email et le cin

int check_is_new_user(user *list, char *cin, char *email) {
    user *courant = list;
    while(courant != NULL) {
        if(strcmp(cin,courant->cin) == 0 || strcmp(email,courant->email) == 0)
            return 0;
        courant = courant->suivant;
    }
    return 1;
}

// fonction pour verifier si l'email donner par un utilisateur est valid ou non

int is_valid_email(char *email){
    for(int i=0;i<strlen(email);i++) {
        if(email[i] == '@')
            return 1;
    }
    return 0;
} 

// fonction pour qui fait le sign up d'un utilisateur

int user_sign_up(user *list, user *u)
{
    if(is_valid_email(u->email) == 0) {
            printf("email non valid!!\n");
            return 0;
    }
    if(check_is_new_user(list,u->cin,u->email) == 0){
        decorate();
        printf("cin or email existe!!!\n");
        return 0;
    }
    list = ajoute_en_fin(list,u);
    return 1;
}

// fonction qui remplir le fichier par la list des utilisateurs

void print_users_list_file(user *list){
    FILE *p_file_user = fopen(user_nom_file, "w+");
    if(!p_file_user){
        printf("ERREUR");
    }
    else{
        user *courant = list;
        while(courant) {
            fprintf(p_file_user,"%s\n%s\n%s\n%s\n%s\n%s\n",courant->cin,courant->nom,courant->prenom,courant->email,courant->telephone,courant->password);
            courant = courant->suivant;
        }
        fclose(p_file_user);
    }
}

void sign_up_menu(user *list) {
    user *u = allocation_dynamique();
    if(!u) {
        printf("ERREUR!!");
    }
    else {
        get_info_sign_up(u);
        if(user_sign_up(list,u) == 1){
            printf("\n\nYou are Sign Up succesfully!!!\n\n");
            print_users_list_file(list);
        }
    }
    system("pause");
}

// fonction pour afficher les informations d'un utilisateur

void *afficher_user_infos(user *u) {
    printf("\33[33m");
    printf("\t\t|%15s|%15s|%15s|%30s|%15s|\n", "---------------", "---------------", "---------------", "------------------------------", "---------------");
    printf("\t\t|%15s|%15s|%15s|%30s|%15s|\n", "CIN", "Nom", "Prenom","Email", "telephone");
    printf("\t\t|%15s|%15s|%15s|%30s|%15s|\n", "---------------", "---------------", "---------------", "------------------------------", "---------------");
    printf("\33[0m");
    printf("\t\t|%15s|%15s|%15s|%30s|%15s|\n",u->cin,u->nom,u->prenom,u->email,u->telephone);
    printf("\33[33m");
    printf("\t\t|%15s|%15s|%15s|%30s|%15s|\n", "---------------", "---------------", "---------------", "------------------------------", "---------------");
    printf("\33[0m");
}

// ****************************************************************************************************
// ***************************************    LOG IN   ************************************************  
// ****************************************************************************************************


user *recherche_user_by_email(user *list, char *email) {
    user *temp = list;
    while(temp) {
        if(strcmp(temp->email, email) == 0)
            return temp;
        temp = temp->suivant;
    }
    return NULL;
}

int check_is_user(user *list, char *email, char *password) {
    user *temp = list;
    while (temp)
    {
        if (strcmp(email, temp->email) == 0 && strcmp(password, temp->password) == 0){
            return 1;
        }
        temp = temp->suivant;
    }
    return 0;
}

int check_is_admin(char *email, char *password){
    FILE *p_file_admin;
    p_file_admin = fopen(admins_nom_file, "r");
    if (p_file_admin == NULL){
        printf("ERREUR!!!");
        return 0;
    }
    user *u = allocation_dynamique();
    while (!feof(p_file_admin))
    {
        fscanf(p_file_admin, "%s\n%s\n%s\n%s\n%s\n%s\n",u->cin, u->nom, u->prenom, u->email, u->telephone, u->password);
        if (strcmp(email, u->email) == 0 && strcmp(password, u->password) == 0) {
            free_user_allocation(u);
            return 1;
        }
    }
    fclose(p_file_admin);
    free_user_allocation(u);
    return 0;
}

void admin_log_in_menu(user *list, user *u, auteur* aut_list) {
    printf("Email : ");
    scanf("%s",u->email);
    printf("Password : ");
    scanf("%s",u->password);
    int check = check_is_admin(u->email, u->password);
    if(check == 1) {
        printf("\nYou are Log in succesfully!!\n");
        after_admin_login_menu(list,aut_list);
    }else   
        printf("\nincorrect information!!\n");
}

void user_log_in_menu(user *list, user *u,auteur *aut_list, categorie* cat_list, livre* livre_arbre) {
    printf("Email : ");
    scanf("%s",u->email);
    printf("Password : ");
    scanf("%s",u->password);
    int check = check_is_user(list, u->email, u->password);
    decorate();
    if(check == 1) {
        printf("\nYou are Log in succesfully!!\n");
        u = recherche_user_by_email(list, u->email);
        system("pause");
        after_user_login_menu(list, u,aut_list,cat_list,livre_arbre);
    }else {
        printf("\nincorrect information!!\n");
        system("pause");
    }
}


// ****************************************************************************************************
// *************************************    CATEGORIES    *********************************************
// ****************************************************************************************************


categorie* ajouter_enfin_cat(categorie* cat_list, categorie *nv_cat) {
    nv_cat->suivant = NULL;
    if(cat_list == NULL) {
        return nv_cat;
    }

    categorie* dernier = cat_list;
    while(dernier->suivant != NULL) {
        dernier = dernier->suivant;
    }

    dernier->suivant = nv_cat;
    return cat_list;
}

categorie* remplir_cat_list(categorie *cat_list) {
    FILE* p_file_cat = fopen(categories_nom_file, "r+");
    if (!p_file_cat) {
        printf("ERREUR!!!");
        return NULL;
    }
    while (!feof(p_file_cat)) {
        categorie *nv_cat = (categorie*)malloc(sizeof(categorie));
        fscanf(p_file_cat,"%d\n%s\n",&nv_cat->id_categorie,nv_cat->intitule);
        cat_list = ajouter_enfin_cat(cat_list,nv_cat);
    }
    fclose(p_file_cat);
    return cat_list;
}

categorie *recherche_cat_by_id(categorie *cat_list, int id_categorie) {
    categorie *cat = cat_list;
    while(cat) {
        if(cat->id_categorie == id_categorie)
            return cat;
        cat = cat->suivant;
    } 
    return NULL;
}

void affiche_categories(categorie *cat_list)
{
    categorie *tmp = cat_list;
    if (cat_list == NULL)
    {
        printf("\033[1;31mil n'y a pas de catégorie!!!\033[0m\n"); // Red text
    }
    else
    {
        printf("\033[33m");
        printf("%c", 201);
        for (int i = 0; i < 102; i++)
        {
            printf("%c", 205);
        }
        printf("%c\n", 187);
        printf("\033[37m");
        printf("                                                          \n");
        printf("\t\t         \033[31m     ~~~ Liste des categories ~~~    \033[37m       \n");
        printf("                                                          \n");
        printf("\033[33m");
        printf("%c", 200);
        for (int i = 0; i < 102; i++)
        {
            printf("%c", 205);
        }
        printf("%c\n", 188);
        printf("\033[37m");
        while (tmp != NULL)
        {
            printf("\t\t\t\t\033[1;32m%d\t%s\033[0m\n", tmp->id_categorie, tmp->intitule); // Green text
            tmp = tmp->suivant;
        }
        printf("\033[33m");
        printf("%c", 201);
        for (int i = 0; i < 102; i++)
        {
            printf("%c", 205);
        }
        printf("%c", 187);
        printf("\033[37m");
        printf("\t\t\t\t                                                     \n");
        printf("\t\t        \033[31m      ~~~ Fin de la liste ~~~  \033[37m              \n");
        printf("\t\t\t\t                                                      \n");
        printf("\033[33m");
        printf("%c", 200);
        for (int i = 0; i < 102; i++)
        {
            printf("%c", 205);
        }
        printf("%c\n", 188);
        printf("\033[37m");
    }
}

void affiche_cat_menu(categorie *cat_list,livre *livre_arbre, auteur *aut_list) {
    decorate();
    int choix_cat;
    categorie *rech;
    do
    {
        affiche_categories(cat_list);
        printf("choisir une categorie : ");
        scanf("%d",&choix_cat);
        rech = recherche_cat_by_id(cat_list, choix_cat);
        if(rech == NULL){
            printf("categorie nexiste pas veullez choisir autre categorie!!!\n");
            decorate();
        }
        else{
            decorate();
            printf("\t\t\t\t\t\033[31m--------------- Categorie %s ---------------\033[37m\n\n",rech->intitule);
            affiche_livre_by_cat(livre_arbre,choix_cat,aut_list);
            system("pause");
        }
    } while (rech == NULL);
}

categorie* supprimer_cat_by_id(categorie* cat_list,int id_cat) {
    categorie* temp;
    categorie* supp;
    if(!cat_list) {
        printf("La liste des categories est vide!!\n");
        return NULL;
    }
    if(cat_list->id_categorie == id_cat) {
        if(cat_list->suivant == NULL)
            return NULL;
        else {
            temp = cat_list->suivant;
            free(cat_list);
            return temp;
        }
    }
    
    temp = cat_list;
    
    while(temp->suivant) {
        if(temp->suivant->id_categorie == id_cat) {
            supp = temp->suivant;
            temp->suivant = supp->suivant;
            free(supp);
            return cat_list;
        }
        temp = temp->suivant;
    }
    return cat_list;
}

categorie *supprimer_cat_by_nom(categorie *cat_list, char *intitule) {
    categorie *supp;
    categorie *tmp;
    if(!cat_list) {
        printf("la liste des categories est vide!!\n");
        return NULL;
    }
    if(strcmp(cat_list->intitule, intitule) == 0) {
        if(cat_list->suivant == NULL)
            return NULL;
        else {
            tmp = cat_list->suivant;
            free(cat_list);
            return tmp;
        }
    }

    tmp = cat_list;

    while(tmp->suivant) {
        if(strcmp(tmp->suivant->intitule, intitule) == 0) {
            supp = tmp->suivant;
            tmp->suivant = supp->suivant;
            free(supp);
            return cat_list;
        }
        tmp = tmp->suivant;
    }

    return cat_list;
}

void enregistrer_cat_in_file(categorie *cat_list) {
    FILE *p_cat_file = fopen(categories_nom_file,"w");
    if(!p_cat_file) {
        printf("ERREUR!!\n");
        return;
    }

    categorie *tmp = cat_list;
    while(tmp) {
        fprintf(p_cat_file,"%d\n%s\n",tmp->id_categorie,tmp->intitule);
        tmp = tmp->suivant;
    }
    fclose(p_cat_file);
}

categorie *recherche_cat_by_nom(categorie *cat_list,char *intitule) {
    categorie *tmp = cat_list;
    while(tmp){
        if(strcmp(tmp->intitule, intitule) == 0) 
            return tmp;
        tmp = tmp->suivant;
    }
    return NULL;
}


// ****************************************************************************************************
// ****************************************    AUTEUR    **********************************************
// ****************************************************************************************************


auteur *ajoute_auteur_enfin(auteur *aut_list, auteur *nv_aut) {
    nv_aut->suivant = NULL;
    if(aut_list == NULL)
        return nv_aut;
    
    auteur *tmp = aut_list;
    while(tmp->suivant) {
        tmp = tmp->suivant;
    }

    tmp->suivant = nv_aut;
    return aut_list;
}

auteur *remplir_auteurs_list(auteur *aut_list) {
    FILE *p_file_auteur = fopen(auteurs_nom_file, "r+");
    if(!p_file_auteur){
        printf("ERREUR!!");
        return NULL;
    }

    while(!feof(p_file_auteur)){
        auteur *tmp = (auteur*)malloc(sizeof(auteur));
        fscanf(p_file_auteur,"%s\n%s\n%d\n",tmp->nom_aut,tmp->prenom_aut,&tmp->id_aut);
        aut_list = ajoute_auteur_enfin(aut_list,tmp);
    }

    fclose(p_file_auteur);
    return aut_list;
}

auteur *recherche_auteur_by_id(auteur *aut_list, int id_auteur) {
    auteur *tmp = aut_list;
    while(tmp) {
        if(tmp->id_aut == id_auteur)
            return tmp;
        tmp = tmp->suivant;
    }
    return NULL;
}

void affichage_auteurs(auteur *list) {
    auteur *tmp=list;
    printf("\t\t\t\t\t--------------- les auteurs disponible ---------------\n");
    while(tmp){
        printf("\t\t\t\t\t\t\t%d : %s %s\n",tmp->id_aut,tmp->nom_aut,tmp->prenom_aut);
        tmp = tmp->suivant;
    }
}

auteur *recherche_aut(auteur *liste_auteur,char* nom,char* prenom) {
    auteur *tmp = liste_auteur;
    while(tmp){
        if(strcmp(tmp->nom_aut,nom)==0 && strcmp(tmp->prenom_aut,prenom)==0)
            return tmp;
        tmp = tmp->suivant;
    }
    return NULL;
}

int generate_id_auteur(auteur* aut_list) {
    int i = 1;
    while(1) {
        if(recherche_auteur_by_id(aut_list,i) == NULL)
            return i;
        i++;
    }
}

auteur *supprimer_aut(auteur *aut_list, char* nom_aut, char* prenom_aut) {
    auteur *temp = aut_list;

    if (strcmp(aut_list->nom_aut, nom_aut) == 0 && strcmp(aut_list->prenom_aut, prenom_aut) == 0){
        temp = aut_list->suivant;
        free(aut_list);
        return temp;
    }

    auteur *rm;
    while (temp->suivant->suivant != NULL) {
        if (strcmp(temp->suivant->nom_aut, nom_aut) == 0 && strcmp(temp->suivant->prenom_aut, prenom_aut) == 0){
            rm = temp->suivant;
            temp->suivant = temp->suivant->suivant;
            free(rm);
            return aut_list;
        }
        temp = temp->suivant;
    }

    if (strcmp(temp->suivant->nom_aut, nom_aut) == 0 && strcmp(temp->suivant->prenom_aut, prenom_aut) == 0){
        free(temp->suivant);
        temp->suivant = NULL;
    }
    return aut_list;
}

void print_aut_in_file(auteur* aut_list) {
    FILE* p_aut_file = fopen(auteurs_nom_file,"w");
    if(!auteurs_nom_file) {
        printf("ERREUR!!\n");
        return;
    }
    auteur *tmp = aut_list;
    while(tmp) {
        fprintf(p_aut_file,"%s\n%s\n%d\n",tmp->nom_aut,tmp->prenom_aut,tmp->id_aut);
        tmp = tmp->suivant;
    }
    fclose(p_aut_file);
}


// ****************************************************************************************************
// ***************************************    LIVRE    ************************************************
// ****************************************************************************************************


livre *creer_noeud_livre(livre *nv) {
    livre *noeud = (livre*)malloc(sizeof(livre));
    strcpy(noeud->nom_livre, nv->nom_livre);
    noeud->id_auteur = nv->id_auteur;
    noeud->date = nv->date;
    noeud->id_categorie = nv->id_categorie;
    noeud->id_livre = nv->id_livre;
    noeud->gauche = NULL;
    noeud->droit = NULL;
    return noeud;
}

livre *ajoute_livre_arbre(livre *racine_arbre, livre *tmp_arbre, livre *nv_livre) {
    if(racine_arbre == NULL)
        return nv_livre;
    if(strcmp(nv_livre->nom_livre,tmp_arbre->nom_livre) == 0)
        return tmp_arbre;
    else if(strcmp(nv_livre->nom_livre,tmp_arbre->nom_livre) < 0){
        if(tmp_arbre->gauche == NULL)
            tmp_arbre->gauche = nv_livre;
        else
            ajoute_livre_arbre(racine_arbre,tmp_arbre->gauche,nv_livre);
    }
    else {
        if(tmp_arbre->droit == NULL)
            tmp_arbre->droit = nv_livre;
        else
            ajoute_livre_arbre(racine_arbre,tmp_arbre->droit,nv_livre);
    }
    return racine_arbre;
}

livre *ajouter_noeud (livre* nv, livre *racine) {
    livre *nouveau = creer_noeud_livre(nv);
    return ajoute_livre_arbre(racine,racine,nouveau);
}

livre *remplir_livre_arbre(livre *livre_arbre){
    FILE *p_file_livre = fopen(livre_nom_file, "r");
    if(!p_file_livre)
        return NULL;
    while(!feof(p_file_livre)){
        livre nv_livre;
        fscanf(p_file_livre,"%s\n%d\n%d\n%d\n%d\n",nv_livre.nom_livre,&nv_livre.date,&nv_livre.id_auteur,&nv_livre.id_categorie,&nv_livre.id_livre);
        livre_arbre = ajouter_noeud(&nv_livre,livre_arbre);
    }
    fclose(p_file_livre);
    return livre_arbre;
}

void affiche_livre_by_cat(livre *livre_arbre,int id_categorie,auteur *aut_list) {
    if(livre_arbre->id_categorie == id_categorie) {
        auteur *tmp_aut = recherche_auteur_by_id(aut_list,livre_arbre->id_auteur);
        printf("\t\t\t\t\t\t\33[33mNom livre  : \33[0m%s - (%d)\n",livre_arbre->nom_livre,livre_arbre->date);
        printf("\t\t\t\t\t\t\33[33mAuteur     : \33[0m%s %s\n",tmp_aut->nom_aut,tmp_aut->prenom_aut);
        printf("\t\t\t\t\t\33[31m===========================================================\33[0m\n");
    }
    if(livre_arbre->gauche != NULL)
        affiche_livre_by_cat(livre_arbre->gauche,id_categorie,aut_list);
    if(livre_arbre->droit != NULL)
        affiche_livre_by_cat(livre_arbre->droit,id_categorie,aut_list);
}

livre *recherche_livre_par_nom(livre *livre_arbre, char *nom_livre) {
    while(livre_arbre != NULL) {
        if(strcmp(livre_arbre->nom_livre,nom_livre) == 0)
            return livre_arbre;
        if(strcmp(livre_arbre->nom_livre,nom_livre) < 0)
            livre_arbre = livre_arbre->droit;
        else
            livre_arbre = livre_arbre->gauche;
    }
    return NULL;
}

livre* recherche_livre_by_id(livre* livre_arbre, int id_livre) {
    if (livre_arbre == NULL)
        return NULL;

    if (livre_arbre->id_livre == id_livre)
        return livre_arbre;

    livre* gauche_livre = recherche_livre_by_id(livre_arbre->gauche, id_livre);
    if (gauche_livre != NULL)
        return gauche_livre;

    livre* droit_livre = recherche_livre_by_id(livre_arbre->droit, id_livre);
    if (droit_livre != NULL)
        return droit_livre;

    return NULL;
}

livre* recherche_livre_by_date(livre* livre_arbre, int date) {
    if (livre_arbre == NULL)
        return NULL;

    if (livre_arbre->date == date)
        return livre_arbre;

    livre* gauche_livre = recherche_livre_by_date(livre_arbre->gauche, date);
    if (gauche_livre != NULL)
        return gauche_livre;

    livre* droit_livre = recherche_livre_by_date(livre_arbre->droit, date);
    if (droit_livre != NULL)
        return droit_livre;

    return NULL;
}

livre* recherche_livre_by_aut(livre* livre_arbre, int id_aut) {
    if (livre_arbre == NULL)
        return NULL;

    if (livre_arbre->id_auteur == id_aut)
        return livre_arbre;

    livre* gauche_livre = recherche_livre_by_aut(livre_arbre->gauche, id_aut);
    if (gauche_livre != NULL)
        return gauche_livre;

    livre* droit_livre = recherche_livre_by_aut(livre_arbre->droit, id_aut);
    if (droit_livre != NULL)
        return droit_livre;

    return NULL;
}

void affiche_livre_by_auteur(livre *livre_arbre,int id_auteur, categorie *cat_list) {
    if(livre_arbre->id_auteur == id_auteur) {
        categorie *tmp_cat = recherche_cat_by_id(cat_list,livre_arbre->id_categorie);
        printf("\t\t\t\t\t\t\33[33mNom livre            :\33[0m %s\n",livre_arbre->nom_livre);
        printf("\t\t\t\t\t\t\33[33mCategorie            :\33[0m %s\n",tmp_cat->intitule);
        printf("\t\t\t\t\t\t\33[33mAnnee de publication :\33[0m %d\n",livre_arbre->date);
        printf("\t\t\t\t\t\33[31m-------------------------------------------------------------\33[0m\n");
    }
    if(livre_arbre->gauche != NULL)
        affiche_livre_by_auteur(livre_arbre->gauche,id_auteur,cat_list);
    if(livre_arbre->droit != NULL)
        affiche_livre_by_auteur(livre_arbre->droit,id_auteur,cat_list);
}

void affiche_livre_by_date(livre *livre_arbre, int date, categorie *cat_list, auteur *aut_list) {
    if(livre_arbre->date == date) {
        categorie *tmp_cat = recherche_cat_by_id(cat_list,livre_arbre->id_categorie);
        auteur *tmp_aut = recherche_auteur_by_id(aut_list,livre_arbre->id_auteur);
        printf("\t\t\t\t\t\t\t\33[33mNom livre  :\33[0m %s\n",livre_arbre->nom_livre);
        printf("\t\t\t\t\t\t\t\33[33mCategorie  :\33[0m %s\n",tmp_cat->intitule);
        printf("\t\t\t\t\t\t\t\33[33mAuteur     :\33[0m %s %s\n",tmp_aut->nom_aut,tmp_aut->prenom_aut);
        printf("\t\t\t\t\t\33[31m-------------------------------------------------------------\33[0m\n");    
    }
    if(livre_arbre->gauche != NULL)
        affiche_livre_by_date(livre_arbre->gauche,date,cat_list,aut_list);
    if(livre_arbre->droit != NULL)
        affiche_livre_by_date(livre_arbre->droit,date,cat_list,aut_list);
}

livre *supprimer_livre_par_nom(livre *racine, char* nom_livre) {
    livre *tmp;
    if (strcmp(racine->nom_livre, nom_livre) == 0) {
        if (racine->gauche != NULL){
            tmp = racine->gauche;
            while (tmp->droit != NULL)
                tmp = tmp->droit;
            tmp->droit = racine->droit;
            racine = racine->gauche;
        }
        else
            racine = racine->droit;
    } 
    else {
        if (strcmp(nom_livre,racine->nom_livre) < 0)
            racine->gauche = supprimer_livre_par_nom(racine->gauche, nom_livre);
        else
            racine->droit = supprimer_livre_par_nom(racine->droit, nom_livre);
    }
    return racine;
}

void print_arbre_in_file(FILE* p_file, livre *livre_arbre) {
    fprintf(p_file, "%s\n%d\n%d\n%d\n%d\n", livre_arbre->nom_livre,livre_arbre->date,livre_arbre->id_auteur,livre_arbre->id_categorie,livre_arbre->id_livre);
    if (livre_arbre->gauche != NULL)
        print_arbre_in_file(p_file,livre_arbre->gauche);
    if (livre_arbre->droit != NULL)
        print_arbre_in_file(p_file,livre_arbre->droit);
}

void enregistrer_arbre_in_file(livre* livre_arbre) {
    livre* tmp = livre_arbre;
    FILE *p_livre_file = fopen(livre_nom_file,"w");
    if(!p_livre_file){
        printf("ERREUR!!\n");
        return;
    }
    print_arbre_in_file(p_livre_file, tmp);
    fclose(p_livre_file);
}

int generate_id_livre(livre* livre_arbre) {
    int i = 1;
    while(1) {
        if(recherche_livre_by_id(livre_arbre,i) == NULL)
            return i;
        i++;
    }
}

livre* recherche_livre_by_cat(livre* livre_arbre, int id_cat) {
    if (livre_arbre == NULL)
        return NULL;

    if (livre_arbre->id_categorie == id_cat) 
        return livre_arbre;

    livre* gauche_livre = recherche_livre_by_cat(livre_arbre->gauche, id_cat);
    if (gauche_livre != NULL)
        return gauche_livre;

    livre* droit_livre = recherche_livre_by_cat(livre_arbre->droit, id_cat);
    if (droit_livre != NULL)
        return droit_livre;

    return NULL;
}

livre* supprimer_livre_by_cat(livre* livre_arbre, int id_cat){
    livre* tmp;
    livre *supp = recherche_livre_by_cat(livre_arbre,id_cat);
    while(supp != NULL) {
        supprimer_demande_by_livre(supp->id_livre);
        supprimer_emprunt_by_livre(supp->id_livre);
        supprimer_emp_refu_by_livre(supp->id_livre);
        supprimer_emp_online_by_livre(supp->id_livre);
        livre_arbre = supprimer_livre_par_nom(livre_arbre,supp->nom_livre);
        tmp = livre_arbre;
        supp = recherche_livre_by_cat(tmp,id_cat);
    }
    return livre_arbre;
}

livre* supprimer_livre_by_aut(livre* livre_arbre, int id_aut){
    livre* tmp;
    livre *supp = recherche_livre_by_aut(livre_arbre,id_aut);
    while(supp != NULL) {
        supprimer_demande_by_livre(supp->id_livre);
        supprimer_emprunt_by_livre(supp->id_livre);
        supprimer_emp_refu_by_livre(supp->id_livre);
        supprimer_emp_online_by_livre(supp->id_livre);
        livre_arbre = supprimer_livre_par_nom(livre_arbre,supp->nom_livre);
        tmp = livre_arbre;
        supp = recherche_livre_by_aut(tmp,id_aut);
    }
    return livre_arbre;
}


// ****************************************************************************************************
// ***********************************    GESTION DES USERS    ****************************************
// ****************************************************************************************************

void afficher_all_users(user *list) {
    user * tmp = list;
    int nb_emp,nb_emp_online,nb_emp_refus;
    printf("\33[31m");
    printf("  |%15s|%15s|%15s|%30s|%15s|%10s|%10s|%10s|\n", "---------------", "---------------", "---------------", "------------------------------", "---------------", "----------", "----------", "----------");
    printf("  |%15s|%15s|%15s|%30s|%15s|%10s|%10s|%10s|\n", "CIN", "Nom", "Prenom","Email", "telephone","nb emp", "emp online", "emp refus");
    printf("  |%15s|%15s|%15s|%30s|%15s|%10s|%10s|%10s|\n", "---------------", "---------------", "---------------", "------------------------------", "---------------", "----------", "----------", "----------");
    while(tmp) {
        nb_emp = nb_emp_user(tmp->cin,0);
        nb_emp_online = nb_emp_user(tmp->cin,1);
        nb_emp_refus = nb_emp_refuser_user(tmp->cin);
        printf("\33[33m");
        printf("  |%15s|%15s|%15s|%30s|%15s|%10d|%10d|%10d|\n",tmp->cin,tmp->nom,tmp->prenom,tmp->email,tmp->telephone,nb_emp,nb_emp_online,nb_emp_refus);
        printf("\33[31m");
        printf("  |%15s|%15s|%15s|%30s|%15s|%10s|%10s|%10s|\n", "---------------", "---------------", "---------------", "------------------------------", "---------------", "----------", "----------", "----------");
        printf("\33[0m");
        tmp = tmp->suivant;
    }
}

void print_list_in_file_user(user *list) {
    FILE *p_file_user = fopen(user_nom_file,"w");
    user *tmp = list;
    while(tmp) {
        fprintf(p_file_user,"%s\n%s\n%s\n%s\n%s\n%s\n",tmp->cin,tmp->nom,tmp->prenom,tmp->email,tmp->telephone,tmp->password);
        tmp = tmp->suivant;
    }
    fclose(p_file_user);
}

void copy_user_infos(user *destination, user *source) {
    strcpy(destination->cin,source->cin);
    strcpy(destination->nom,source->nom);
    strcpy(destination->prenom,source->prenom);
    strcpy(destination->email,source->email);
    strcpy(destination->telephone,source->telephone);
    strcpy(destination->password,source->password);
}

int user_info_modification(user *list_user, user *u, user *nv) {
    user *tmp = list_user;
    while(tmp) {
        if(strcmp(tmp->cin, u->cin) == 0) {
            copy_user_infos(tmp,nv);
            return 1;
        }            
        tmp = tmp->suivant;
    }
    return 0;
}

void modification_user_menu(user *list_user, user *u) {
    decorate();
    int option,test;
    user *nv = allocation_dynamique();
    copy_user_infos(nv,u);
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                      MODIFICATION USER MENU                                                |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                  [\033[32m1\033[31m]\033[33m modifier tous les informations                                        \033[32m|\n");
    printf("\t\t|\033[31m                                  [\033[32m2\033[31m]\033[33m modifier le nom                                                       \033[32m|\n");
    printf("\t\t|\033[31m                                  [\033[32m3\033[31m]\033[33m modifier le prenom                                                    \033[32m|\n");
    printf("\t\t|\033[31m                                  [\033[32m4\033[31m]\033[33m modifier le email                                                     \033[32m|\n");
    printf("\t\t|\033[31m                                  [\033[32m5\033[31m]\033[33m modifier le telephone                                                 \033[32m|\n");
    printf("\t\t|\033[31m                                  [\033[32m6\033[31m]\033[33m modifier le mot de passe                                              \033[32m|\n");
    printf("\t\t|\033[31m                                  [\033[32m7\033[31m]\033[33m Retourner                                                             \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("choisir une option : ");
    scanf("%d",&option);

    decorate();

    switch (option)
    {
    case 1:
        printf("nouveau nom : ");
        scanf("%s",nv->nom);
        printf("nouveau prenom : ");
        scanf("%s",nv->prenom);
        printf("nouveau email : ");
        scanf("%s",nv->email);
        printf("nouveau telephone :");
        scanf("%s",nv->telephone);
        printf("nouveau mot de pass : ");
        scanf("%s",nv->password);
        test = user_info_modification(list_user,u,nv);
        if(test == 1) {
            printf("\nTes information est modifier avec succes!!\n");
        }
        else 
            printf("Erreur de modification!!\n");
        break;
    
    case 2:
        printf("nouveau nom : ");
        scanf("%s",nv->nom);
        test = user_info_modification(list_user,u,nv);
        if(test == 1)
            printf("\nTon nom est modifier avec succes!!\n");
        else 
            printf("Erreur de modification!!");
        break;

    case 3:
        printf("nouveau prenom : ");
        scanf("%s",nv->prenom);
        test = user_info_modification(list_user,u,nv);
        if(test == 1)
            printf("\nTon prenom est modifier avec succes!!\n");
        else 
            printf("\nErreur de modification!!\n");
        break;

    case 4:
        printf("nouveau Email : ");
        scanf("%s",nv->email);
        test = user_info_modification(list_user,u,nv);
        if(test == 1)
            printf("\nTon Email est modifier avec succes!!\n");
        else 
            printf("\nErreur de modification!!\n");
        break;

    case 5:
        printf("nouveau Telephone : ");
        scanf("%s",nv->telephone);
        test = user_info_modification(list_user,u,nv);
        if(test == 1)
            printf("\nTon telephone est modifier avec succes!!\n");
        else 
            printf("\nErreur de modification!!\n");
        break;

    case 6:
        printf("nouveau Mot de pass : ");
        scanf("%s",nv->password);
        test = user_info_modification(list_user,u,nv);
        if(test == 1)
            printf("\nTon Mot de pass est modifier avec succes!!\n");
        else 
            printf("\nErreur de modification!!\n");
        break;

    case 7:
        break;
    default:
        break;
    }
    printf("voulez vous enregistrer les modifications : \n");
    printf("1- Enregistrer\n");
    printf("2- Annuler\n");
    printf("choisir une option : ");
    scanf("%d",&option);
    if(option == 1){
        copy_user_infos(u,nv);
        print_list_in_file_user(list_user);
    }
    else{
        printf("Operaion annuler!!\n");
    }
}

user *supprimer_compte(user *user_list, user *u) {
    user *tmp;
    user *supp;
    if(strcmp(user_list->cin,u->cin) == 0){
        tmp = user_list->suivant;
        free(user_list);
        return tmp;
    }

    tmp = user_list;
    while(tmp->suivant) {
        if(strcmp(tmp->suivant->cin,u->cin) == 0){
            supp = tmp->suivant;
            tmp->suivant = supp->suivant;
            free(supp);
            return user_list;
        }
        tmp = tmp->suivant;
    }
    return user_list;
}

user *recherche_user_par_cin(user *list, char *cin) {
    user *temp = list;
    while(temp) {
        if(strcmp(temp->cin, cin) == 0)
            return temp;
        temp = temp->suivant;
    }
    return NULL;
}

user *recherche_user_par_prenom(user *list, char *prenom) {
    user *temp = list;
    while(temp) {
        if(strcmp(temp->prenom, prenom) == 0)
            return temp;
        temp = temp->suivant;
    }
    return NULL;
}

user *recherche_user_par_nom(user *list, char *nom) {
    user *temp = list;
    while(temp) {
        if(strcmp(temp->nom, nom) == 0)
            return temp;
        temp = temp->suivant;
    }
    return NULL;
}


// ****************************************************************************************************
// ***********************************    DEMANDE EMPRUNT    ******************************************
// ****************************************************************************************************


int enregistrer_demande(livre *emprunt_livre, user *u) {
    FILE *p_file_demande = fopen(demandes_nom_file, "a+");
    if(!p_file_demande)
        return 0;
    fprintf(p_file_demande,"%s\n%d\n",u->cin,emprunt_livre->id_livre);
    fclose(p_file_demande);
    return 1;
}

demande *recherche_demande(char *cin) {
    FILE *p_file_demande = fopen(demandes_nom_file,"r");
    demande *tmp_dem = (demande*)malloc(sizeof(demande));
    if(!p_file_demande)
        return NULL;
    while(!feof(p_file_demande)) {
        fscanf(p_file_demande,"%s\n%d\n",tmp_dem->cin,&tmp_dem->id_livre);
        if(strcmp(tmp_dem->cin, cin) == 0){
            fclose(p_file_demande);
            return tmp_dem;
        }
    }
    fclose(p_file_demande);
    return NULL;
}

int supprimer_demande(demande *supp_dem) {
    FILE *p_file_demande = fopen(demandes_nom_file, "r");
    if(!p_file_demande)
        return 0;
    FILE *p_tmp = fopen("temporaire.txt", "w");
    if(!p_tmp)
        return 0;
    demande *tmp_dem = (demande*)malloc(sizeof(demande)); 
    while(!feof(p_file_demande)) {
        fscanf(p_file_demande,"%s\n%d\n",tmp_dem->cin,&tmp_dem->id_livre);
        if(strcmp(tmp_dem->cin,supp_dem->cin) != 0)
            fprintf(p_tmp,"%s\n%d\n",tmp_dem->cin,tmp_dem->id_livre);
    }
    fclose(p_file_demande);
    fclose(p_tmp);
    remove(demandes_nom_file);
    rename("temporaire.txt",demandes_nom_file);
    return 1;
}

void affiche_demande_refuser(user *u,livre *livre_arbre) {
    FILE *p_file_refu_dem = fopen(demande_refuse_nom_file,"r");
    if(!p_file_refu_dem){
        printf("ERREUR!!!\n");
        return;
    }   
    int temp = 0;
    emprunts *tmp_dem = (emprunts*)malloc(sizeof(emprunts));
    while(!feof(p_file_refu_dem)) {
        fscanf(p_file_refu_dem,"%s\n%d\n%s\n",tmp_dem->cin,&tmp_dem->id_livre,tmp_dem->date_debut);
        if(strcmp(u->cin,tmp_dem->cin) == 0){
            livre *tmp_livre = recherche_livre_by_id(livre_arbre,tmp_dem->id_livre);
            printf("\t\t\t\t\t\t\33[31m---------------------------------------------------\33[0m\n");
            printf("\t\t\t\t\t\t\t%s  %s\n",tmp_livre->nom_livre,tmp_dem->date_debut);
            printf("\t\t\t\t\t\t\33[31m---------------------------------------------------\33[0m\n");
            temp++;
        }
    }
    if(temp == 0)
        printf("\33[31mvous n avez pas de demandes refuser!!!\33[0m\n");
}

void supprimer_demande_by_livre(int id_livre) {
    char cin[30];
    int id;
    FILE *p_demande_file = fopen(demandes_nom_file,"r");
    if(!p_demande_file){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_demande_file)){
        fscanf(p_demande_file,"%s\n%d\n",cin,&id);
        if(id_livre != id && id != 0)
            fprintf(p_tmp,"%s\n%d\n",cin,id);
    }
    fclose(p_demande_file);
    fclose(p_tmp);
    remove(demandes_nom_file);
    rename("test.txt",demandes_nom_file);
}

void supprimer_emp_refu_by_livre(int id_livre) {
    char cin[30];
    int id;
    FILE *p_refu = fopen(demande_refuse_nom_file,"r");
    if(!p_refu){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_refu)){
        fscanf(p_refu,"%s\n%d\n",cin,&id);
        if(id_livre != id && id != 0)
            fprintf(p_tmp,"%s\n%d\n",cin,id);
    }
    fclose(p_refu);
    fclose(p_tmp);
    remove(demande_refuse_nom_file);
    rename("test.txt",demande_refuse_nom_file);
}

void supprimer_demande_by_cin(char *supp_cin) {
    char cin[30];
    int id;
    FILE *p_demande_file = fopen(demandes_nom_file,"r");
    if(!p_demande_file){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_demande_file)){
        fscanf(p_demande_file,"%s\n%d\n",cin,&id);
        if(strcmp(cin,supp_cin) != 0 && id != 0)
            fprintf(p_tmp,"%s\n%d\n",cin,id);
    }
    fclose(p_demande_file);
    fclose(p_tmp);
    remove(demandes_nom_file);
    rename("test.txt",demandes_nom_file);
}

void supprimer_demande_refu_by_cin(char *supp_cin) {
    char cin[30],date_refu[11];
    int id;
    FILE *p_demande_file = fopen(demande_refuse_nom_file,"r");
    if(!p_demande_file){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_demande_file)){
        fscanf(p_demande_file,"%s\n%d\n%s\n",cin,&id,date_refu);
        if(strcmp(cin,supp_cin) != 0 && id != 0)
            fprintf(p_tmp,"%s\n%d\n%s\n",cin,id,date_refu);
    }
    fclose(p_demande_file);
    fclose(p_tmp);
    remove(demande_refuse_nom_file);
    rename("test.txt",demande_refuse_nom_file);
}

void print_demande_in_file(file *dem_file) {
    FILE *p_demande_file = fopen(demandes_nom_file, "w");
    if(!p_demande_file) {
        printf("ERREUR!!\n");
        return;
    }
    demande *tmp = dem_file->tete;
    while(tmp) {
        fprintf(p_demande_file,"%s\n%d\n",tmp->cin,tmp->id_livre);
        tmp = tmp->suivant;
    }
    fclose(p_demande_file);
}

void affiche_all_demandes(char *nom_file, livre* livre_arbre) {
    FILE *p_file;
    p_file = fopen(nom_file, "r");
    emprunts emp;
    livre *tmp;
    int trouve = 0;
    while (!feof(p_file))
    {
        fscanf(p_file, "%s\n%d\n%s\n", emp.cin, &emp.id_livre, emp.date_debut);
        tmp = recherche_livre_by_id(livre_arbre,emp.id_livre);
        if(tmp != NULL) {
            printf("\t\t\t\t\t\t\t\33[33mClient        :\33[0m %s\n",emp.cin);
            printf("\t\t\t\t\t\t\t\33[33mnom livre     :\33[0m %s\n",tmp->nom_livre);
            printf("\t\t\t\t\t\t\t\33[33mdate refus    :\33[0m %s\n",emp.date_debut);
            printf("\t\t\t\t\t\t\33[31m---------------------------------------\33[0m\n");
            trouve =1;
        }
    }
    if(trouve == 0)
        printf("\33[31mla liste est vide!!\33[0m\n");
    fclose(p_file);
}


// ****************************************************************************************************
// ***************************************     EMPRUNT    *********************************************
// ****************************************************************************************************

int nb_emp_refuser_user(char *cin_user) {
    FILE *p = fopen(demande_refuse_nom_file,"r+");
    if(!p)
        return 0;
    int nb = 0;
    char cin[30],date[11];
    int id_livre;
    while(!feof(p)) {
        fscanf(p,"%s\n%d\n%s\n",cin,&id_livre,date);
        if(strcmp(cin_user,cin) == 0)
            nb++;
    }
    fclose(p);
    return nb;
}

int nb_emp_user(char *cin, int choix){
    FILE *p1;
    int n = 0;
    emprunts nv;
    if (choix == 0){
        p1 = fopen(emprunts_nom_file, "r");
        if (p1 == NULL)
            return 0;
    }
    else{
        p1 = fopen(emprunts_online_nom_file, "r");
        if (p1 == NULL)
            return 0;
    }
    while (!feof(p1)){
        fscanf(p1, "%s\n%d\n%s\n%s\n", nv.cin, &nv.id_livre, nv.date_debut, nv.date_fin);
        if (strcmp(nv.cin, cin) == 0)
            n++;
    }
    fclose(p1);
    return n;
}

int supprimer_emp_online_depasser() {
    FILE* p_emp_file = fopen(emprunts_online_nom_file,"r");
    if(!p_emp_file)
        return -1;
    FILE* p_test = fopen("test.txt","w");
    if(!p_test)
        return -1;
    emprunts emp;
    while(!feof(p_emp_file)) {
        fscanf(p_emp_file,"%s\n%d\n%s\n%s\n",emp.cin,&emp.id_livre,emp.date_debut,emp.date_fin);
        if(strcmp(emp.date_fin,get_current_date()) > 0)
            fprintf(p_test,"%s\n%d\n%s\n%s\n",emp.cin,emp.id_livre,emp.date_debut,emp.date_fin);
    }
    fclose(p_emp_file);
    fclose(p_test);
    remove(emprunts_online_nom_file);
    rename("test.txt",emprunts_online_nom_file);
    return 0;
}

emprunts *recherche_emprunt_online(char *cin) {
    FILE *p_file_online_emp = fopen(emprunts_online_nom_file,"r");
    emprunts *tmp_online_emp = (emprunts*)malloc(sizeof(emprunts));
    if(!p_file_online_emp)
        return NULL;
    while(!feof(p_file_online_emp)) {
        fscanf(p_file_online_emp,"%s\n%d\n%s\n%s\n",tmp_online_emp->cin,&tmp_online_emp->id_livre,tmp_online_emp->date_debut,tmp_online_emp->date_fin);
        if(strcmp(tmp_online_emp->cin, cin) == 0){
            fclose(p_file_online_emp);
            return tmp_online_emp;
        }
    }
    fclose(p_file_online_emp);
    return NULL;
}

emprunts *recherche_emprunt(char *cin) {
    FILE *p_file_online_emp = fopen(emprunts_nom_file,"r");
    emprunts *tmp_emp = (emprunts*)malloc(sizeof(emprunts));
    if(!p_file_online_emp)
        return NULL;
    while(!feof(p_file_online_emp)) {
        fscanf(p_file_online_emp,"%s\n%d\n%s\n%s\n",tmp_emp->cin,&tmp_emp->id_livre,tmp_emp->date_debut,tmp_emp->date_fin);
        if(strcmp(tmp_emp->cin, cin) == 0){
            fclose(p_file_online_emp);
            return tmp_emp;
        }
    }
    fclose(p_file_online_emp);
    return NULL;
}

void affiche_user_emprunt(char* cin,livre *livre_arbre) {
    emprunts *tmp_emp = (emprunts*)malloc(sizeof(emprunts));
    FILE *p_file_emprunt = fopen(emprunts_nom_file,"r");
    int trouve = 0;
    if(!p_file_emprunt)
        printf("Erreur!!");
    else{
        printf("\t\t\t\t\t\t\33[31m------------- voila votre emprunt ---------------\33[0m\n");
        while(!feof(p_file_emprunt)){
            fscanf(p_file_emprunt,"%s\n%d\n%s\n%s\n",tmp_emp->cin,&tmp_emp->id_livre,tmp_emp->date_debut,tmp_emp->date_fin);
            if(strcmp(cin, tmp_emp->cin) == 0) {
                livre *tmp_livre = livre_arbre;
                livre *l = recherche_livre_by_id(livre_arbre,tmp_emp->id_livre);
                printf("\t\t\t\t\t\t\t%s %s  %s\n",l->nom_livre,tmp_emp->date_debut,tmp_emp->date_fin);
                trouve = 1;
            }   
        }
        if(trouve == 0)
            printf("\t\t\t\t\t\t\tvous n avez pas des emprunts!!\n");
        printf("\t\t\t\t\t\t\33[31m--------------------------------------------------\33[0m\n");
    }
    fclose(p_file_emprunt);
    system("pause");
}

void supprimer_emprunt_by_livre(int id_livre) {
    char cin[30],date_deb[11],date_fin[11];
    int id;
    FILE *p_emp_file = fopen(emprunts_nom_file,"r");
    if(!p_emp_file){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_emp_file)){
        fscanf(p_emp_file,"%s\n%d\n%s\n%s\n",cin,&id,date_deb,date_fin);
        if(id_livre != id && id != 0)
            fprintf(p_tmp,"%s\n%d\n",cin,id,date_deb,date_fin);
    }
    fclose(p_emp_file);
    fclose(p_tmp);
    remove(emprunts_nom_file);
    rename("test.txt",emprunts_nom_file);
}

void supprimer_emp_online_by_livre(int id_livre) {
    char cin[30],date_deb[11],date_fin[11];
    int id;
    FILE *p_online = fopen(emprunts_online_nom_file,"r");
    if(!p_online){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_online)){
        fscanf(p_online,"%s\n%d\n%s\n%s\n",cin,&id,date_deb,date_fin);
        if(id_livre != id && id != 0)
            fprintf(p_tmp,"%s\n%d\n",cin,id,date_deb,date_fin);
    }
    fclose(p_online);
    fclose(p_tmp);
    remove(emprunts_online_nom_file);
    rename("test.txt",emprunts_online_nom_file);
}

void supprimer_emprunt_by_cin(char *nom_fichier,char *supp_cin) {
    char cin[30],date_deb[11],date_fin[11];
    int id;
    FILE *p_emp_file = fopen(nom_fichier,"r");
    if(!p_emp_file){
        printf("ERREUR!!\n");
        return;
    }
    FILE *p_tmp = fopen("test.txt", "w");
    if(!p_tmp){
        printf("ERREUR!!\n");
        return;
    }
    while(!feof(p_emp_file)){
        fscanf(p_emp_file,"%s\n%d\n%s\n%s\n",cin,&id,date_deb,date_fin);
        if(strcmp(cin,supp_cin) != 0 && id != 0)
            fprintf(p_tmp,"%s\n%d\n%s\n%s\n",cin,id,date_deb,date_fin);
    }
    fclose(p_emp_file);
    fclose(p_tmp);
    remove(nom_fichier);
    rename("test.txt",nom_fichier);
}

int affiche_emp_by_time(char *date,livre* livre_arbre, int option) {
    FILE *p_file;
    p_file = fopen(emprunts_nom_file, "r");
    emprunts emp;
    livre *tmp;
    int trouve = 0;
    while (!feof(p_file))
    {
        fscanf(p_file, "%s\n%d\n%s\n%s\n", emp.cin, &emp.id_livre, emp.date_debut, emp.date_fin);
        tmp = recherche_livre_by_id(livre_arbre,emp.id_livre);
        if(strcmp(emp.date_debut,date) == option && tmp != NULL) {
            printf("\t\t\t\t\t\t\t\33[33mClient        :\33[0m %s\n",emp.cin);
            printf("\t\t\t\t\t\t\t\33[33mnom livre     :\33[0m %s\n",tmp->nom_livre);
            printf("\t\t\t\t\t\t\t\33[33mDebut Emprunt :\33[0m %s\n",emp.date_debut);
            printf("\t\t\t\t\t\t\t\33[33mFin emprunt   :\33[0m %s\n",emp.date_fin);
            printf("\t\t\t\t\t\t\33[31m---------------------------------------------\33[0m\n");
            trouve = 1;
        }
    }
    fclose(p_file);
    return trouve;
}

void affiche_all_emprunts(char *nom_file, livre* livre_arbre) {
    FILE *p_file;
    p_file = fopen(nom_file, "r");
    emprunts emp;
    livre *tmp;
    int trouve = 0;
    while (!feof(p_file))
    {
        fscanf(p_file, "%s\n%d\n%s\n%s\n", emp.cin, &emp.id_livre, emp.date_debut, emp.date_fin);
        tmp = recherche_livre_by_id(livre_arbre,emp.id_livre);
        if(tmp != NULL) {
            printf("\t\t\t\t\t\t\t\33[33m Client        :\33[0m %s\n",emp.cin);
            printf("\t\t\t\t\t\t\t\33[33m nom livre     :\33[0m %s\n",tmp->nom_livre);
            printf("\t\t\t\t\t\t\t\33[33m Debut Emprunt :\33[0m %s\n",emp.date_debut);
            printf("\t\t\t\t\t\t\t\33[33m Fin emprunt   :\33[0m %s\n",emp.date_fin);
            printf("\t\t\t\t\t\t\33[31m---------------------------------------\33[0m\n");
            trouve = 1;
        }
    }
    if(trouve == 0) 
        printf("\33[31mla liste des emprunts est vide!!\33[0m\n");
    fclose(p_file);
}


// ****************************************************************************************************
// ***************************************     LES MENU    ********************************************
// ****************************************************************************************************


void demande_emprunt_menu(livre *livre_arbre, categorie *cat_list,user *user_list, user *u,auteur *aut_list){    
    livre *tmp_livre = livre_arbre;
    char *nom_livre = (char*)malloc(30*sizeof(char));
    affiche_cat_menu(cat_list,tmp_livre,aut_list);
    printf("Donner le nom du livre : ");
    scanf("%s",nom_livre);
    decorate();
    livre *emprunt_livre = recherche_livre_par_nom(tmp_livre,nom_livre);
    if( emprunt_livre == NULL)
        printf("livre invalid!!!\n");
    else{
        int check = enregistrer_demande(emprunt_livre,u);
        if(check == 1){
            printf("Votre demande Emprunt du livre : %s a ete envoyer\n",nom_livre);
            printf("Veuillez attendez Acceptation du ADMIN\n");
        }else
            printf("ENVOIE DU DEMANDE ECHEC\n");
    }
}

void gestion_mes_demande(livre *livre_arbre, categorie *cat_list, user *user_list, user *u, auteur *aut_list) {
    decorate();
    int option,check;
    demande *tmp_dem;
    emprunts *tmp_emp;
    livre *tmp_livre;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                  GESTION DES DEMANDES D'EMPRUNT                                            |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                [\033[32m1\033[31m]\033[33m Effectuer une demande Emprunt                                           \033[32m|\n");
    printf("\t\t|\033[31m                                [\033[32m2\033[31m]\033[33m Supprimer ma demande                                                    \033[32m|\n");
    printf("\t\t|\033[31m                                [\033[32m3\033[31m]\033[33m Afficher mes demandes refuse                                            \033[32m|\n");
    printf("\t\t|\033[31m                                [\033[32m4\033[31m]\033[33m Afficher ma demande en attente                                          \033[32m|\n");
    printf("\t\t|\033[31m                                [\033[32m5\033[31m]\033[33m Afficher mon emprunt en ligne                                           \033[32m|\n");
    printf("\t\t|\033[31m                                [\033[32m6\033[31m]\033[33m Retourner                                                               \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("choisir une option : ");
    scanf("%d",&option);

    decorate();

    tmp_dem = recherche_demande(u->cin);
    tmp_emp = recherche_emprunt_online(u->cin);

    switch (option){

    case 1:
        if(tmp_dem != NULL)
            printf("\33[31mTu as deja une demande en attend\33[0m\n");
        else if(tmp_emp != NULL)
            printf("T\33[31mu as deja une emprunt en ligne\33[0m\n");
        else
            demande_emprunt_menu(livre_arbre,cat_list,user_list,u,aut_list);
        break;
    
    case 2:
        if(tmp_dem == NULL)
            printf("\33[31mvous n avez pas de demandes en attend\33[0m\n");
        else{
            tmp_livre = recherche_livre_by_id(livre_arbre,tmp_dem->id_livre);
            check = supprimer_demande(tmp_dem);
            if(check == 1) {
                printf("\t\t\t\t\t\t\33[31m------- vous avez supprimer la demande ---------\33[0m\n");
                printf("\t\t\t\t\t\t\t%s  %s\n",tmp_dem->cin,tmp_livre->nom_livre);
                printf("\t\t\t\t\t\t\33[31m------------------------------------------------\33[0m\n");
            }else {
                printf("ERREUR!!!\n");
            }
        }
        break;

    case 3:
        affiche_demande_refuser(u,livre_arbre);
        break;

    case 4:
        if(tmp_dem == NULL)
            printf("\33[31mvous n'avez aucun demande en attend!!\33[0m\n");
        else{
            tmp_livre = recherche_livre_by_id(livre_arbre,tmp_dem->id_livre);
            printf("\t\t\t\t\t\t\33[31m------------ votre demande en attend ------------\33[0m\n");
            printf("\t\t\t\t\t\t\t%s  %s\n",tmp_dem->cin,tmp_livre->nom_livre);
            printf("\t\t\t\t\t\t\33[31m-------------------------------------------------\33[0m\n");
        }
        break;

    case 5:
        if(tmp_emp == NULL)
            printf("\33[31mvous n'avez aucun demande en ligne!!\33[0m\n");
        else {
            tmp_livre = recherche_livre_by_id(livre_arbre,tmp_emp->id_livre);
            printf("\t\t\t\t\t\t\33[31m------------ votre demande en en ligne ------------\33[0m\n");
            printf("\t\t\t\t\t\t\t%s  %s\n",tmp_livre->nom_livre,tmp_emp->date_debut,tmp_emp->date_fin);
            printf("\t\t\t\t\t\t\33[31m---------------------------------------------------\33[0m\n");
        }
        break;

    case 6:
        after_user_login_menu(user_list,u,aut_list,cat_list,livre_arbre);

    default:

        break;
    }
    system("pause");
    gestion_mes_demande(livre_arbre,cat_list,user_list,u,aut_list);

}

void recherche_livre_menu(livre *livre_arbre, user *user_list,categorie *cat_list,auteur *aut_list) {
    decorate();
    int option;
    auteur *tmp_aut;
    categorie *tmp_cat;
    livre *l = livre_arbre;

    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                              RECHERCHE LIVRE                                               |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                     [\033[32m1\033[31m]\033[33m recherche par nom de livre                                         \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m2\033[31m]\033[33m recherche par nom Auteur                                           \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m3\033[31m]\033[33m recherche par date de publication                                  \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m4\033[31m]\033[33m Retourner                                                          \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("choisir une option : ");
    scanf("%d",&option);

    decorate();

    switch (option){

    case 1:
        char nom_livre[50];
        printf("Donner le nom de livre : ");
        scanf("%s",nom_livre);
        decorate();
        l = recherche_livre_par_nom(l,nom_livre);
        if(l == NULL)
            printf("\n\nLivre non trouver!!\n\n");
        else {
            tmp_aut = recherche_auteur_by_id(aut_list,l->id_auteur);
            tmp_cat = recherche_cat_by_id(cat_list,l->id_categorie);
            printf("\t\t\t\t\t\t\033[31m-------------------------------------------\033[37m\n");
            printf("\t\t\t\t\t\t\033[33m  le nom de livre        :\033[37m %s\n",l->nom_livre);
            printf("\t\t\t\t\t\t\033[33m  la date de publication :\033[37m %d\n",l->date);
            printf("\t\t\t\t\t\t\033[33m  Categorie du livre     :\033[37m %s\n",tmp_cat->intitule);
            printf("\t\t\t\t\t\t\033[33m  Auteur de livre        :\033[37m %s %s\n",tmp_aut->nom_aut,tmp_aut->prenom_aut);
            printf("\t\t\t\t\t\t\033[31m-------------------------------------------\033[37m\n");
        }
        break;
    
    case 2:
        int id_auteur;
        affichage_auteurs(aut_list);
        printf("Choisir l'id d'Auteur : ");
        scanf("%d",&id_auteur);
        decorate();
        decorate();
        tmp_aut = recherche_auteur_by_id(aut_list,id_auteur);
        if(tmp_aut == NULL) 
            printf("Auteur non trouver !!!");
        else{
            printf("\t\t\t\t\t\33[31m------------------- les livre de %s %s--------------\33[0m\n",tmp_aut->nom_aut,tmp_aut->prenom_aut);
            affiche_livre_by_auteur(livre_arbre,id_auteur,cat_list);
        }
        break;

    case 3:
        int date;
        printf("Donner la date du publication : ");
        scanf("%d",&date);
        decorate();
        l = recherche_livre_by_date(l,date);
        if(l==NULL)
            printf("Aucun livre ni publier dans cette date!!\n");
        else {
            printf("\t\t\t\t\t\33[31m--------------------- les livre publier on %d -------------\33[0m\n",date);
            affiche_livre_by_date(livre_arbre,date,cat_list,aut_list);
        }
        break;

    case 4:
        return;
    default:
        printf("option invalid!!!\n");
        break;
    }
    system("pause");
    recherche_livre_menu(livre_arbre,user_list,cat_list,aut_list);
}

void supprimer_livre_menu(livre *livre_arbre,user *user_list, auteur* aut_list, categorie* cat_list){
    decorate();
    int id_livre;
    char nom_livre[20];
    int op;
    livre *test;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                          SUPPRIMER LIVRE MENU                                              |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                      [\033[32m1\033[31m]\033[33m Supprimer le livre par id                                         \033[32m|\n");
    printf("\t\t|\033[31m                                      [\033[32m2\033[31m]\033[33m Supprimer le livre par nom                                        \033[32m|\n");
    printf("\t\t|\033[31m                                      [\033[32m3\033[31m]\033[33m Retourner                                                         \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisissez une option : ");
    scanf("%d", &op);
    decorate();
    switch (op) {

    case 1:
        printf("Donner l'id de livre que vous voulez supprimer : ");
        scanf("%d", &id_livre);
        test = recherche_livre_by_id(livre_arbre,id_livre);
        break;

    case 2:
        printf("Donner le nom de livre que vous voulez supprimer : ");
        scanf("%s", nom_livre);
        test = recherche_livre_par_nom(livre_arbre,nom_livre);
        break;
    case 3:
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
        break;
    default:
        printf("le choix est invalid \n");
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
        break;
    }


    if (test == NULL) {
            printf("le livre n'existe pas!!\n");
    }
    else{
        printf("Tous les demande et les emprunt de ce livre sera supprimee!!!\n");
        printf("1- continuer : ");
        scanf("%d",&op);
        if(op == 1) {
            supprimer_demande_by_livre(test->id_livre);
            supprimer_emprunt_by_livre(test->id_livre);
            supprimer_emp_refu_by_livre(test->id_livre);
            supprimer_emp_online_by_livre(test->id_livre);
            livre_arbre = supprimer_livre_par_nom(livre_arbre, test->nom_livre);
            enregistrer_arbre_in_file(livre_arbre);
            printf("le livre %s a bien supprimer \n", test->nom_livre);
        }
        else 
            printf("Opertion annuller!!\n");
    }
}

void ajoute_livre_menu(livre *livre_arbre, user* user_list, auteur* aut_list,categorie* cat_list) {
    decorate();
    livre *nv_livre = (livre*)malloc(sizeof(livre));
    printf("Donner le nom de livre  : ");
    scanf("%s", nv_livre->nom_livre);
    if (recherche_livre_par_nom(livre_arbre, nv_livre->nom_livre) != NULL) {
        printf("le livre %s existe deja !!\n", nv_livre->nom_livre);
        system("pause");
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
    }

    printf("Donner la date de publication : ");
    scanf("%d", &nv_livre->date);

    printf("Donner l'id d'auteur qui a ecrit ce livre :");
    scanf("%d", &nv_livre->id_auteur);
    if(recherche_auteur_by_id(aut_list,nv_livre->id_auteur) == NULL){
        printf("Auteur non trouvee!!\n");
        system("pause");
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
    }
    
    printf("donner l'id' du categorie du livre :");
    scanf("%d", &nv_livre->id_categorie);
    if(recherche_cat_by_id(cat_list,nv_livre->id_categorie) == NULL){
        printf("categorie non trouvee!!\n");
        system("pause");
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
    }
    nv_livre->id_livre = generate_id_livre(livre_arbre);
    livre_arbre = ajouter_noeud(nv_livre,livre_arbre);
    enregistrer_arbre_in_file(livre_arbre);
    printf("l'ajoue a effectue avec succes !! \n");
    system("pause");
    menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
}

void modification_livre_menu(livre* livre_arbre,user* user_list,auteur* aut_list, categorie* cat_list) {
    decorate();
    int option;
    livre* nv_livre = (livre*)malloc(sizeof(livre));
    int old_id_livre;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                     MODIFICATION DU LIVRE MENU                                             |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                 [\033[32m1\033[31m]\033[33m Modifier tous les infos du livre                                       \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m2\033[31m]\033[33m Modifier le nom du livre                                               \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m3\033[31m]\033[33m Modifier la date du publication du livre                               \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m4\033[31m]\033[33m Modifier l'auteur du livre                                             \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m5\033[31m]\033[33m Modifier la categorie du livre                                         \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m6\033[31m]\033[33m Retour                                                                 \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &option);
    decorate();
    printf("donner id du livre a modifier : ");
    scanf("%d",&old_id_livre);
    decorate();
    livre* rech = recherche_livre_by_id(livre_arbre,old_id_livre);

    if(rech == NULL){
        printf("Livre non trouvee!!!\n");
        return;
    }

    nv_livre->id_livre = rech->id_livre;
    strcpy(nv_livre->nom_livre,rech->nom_livre);
    nv_livre->id_auteur = rech->id_auteur;
    nv_livre->id_categorie = rech->id_categorie;
    nv_livre->date = rech->date;

    switch (option){

    case 1:
        printf("\n");
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("\t\t|                                            MODIFICATION COMPLETE                                           |\n"); 
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("nouveau nom de livre : ");
        scanf("%s",nv_livre->nom_livre);
        if(recherche_livre_par_nom(livre_arbre,nv_livre->nom_livre) != NULL){
            printf("Livre existe!!!\n");
            return;
        }

        printf("nouveau date de publication : ");
        scanf("%d",&nv_livre->date);

        printf("nouveau id auteur : ");
        scanf("%d",&nv_livre->id_auteur);
        if(recherche_auteur_by_id(aut_list,nv_livre->id_auteur) == NULL){
            printf("Auteur n existe pas!!\n");
            return;
        }

        printf("nouveau categorie : ");
        scanf("%d",&nv_livre->id_categorie);
        if(recherche_cat_by_id(cat_list,nv_livre->id_categorie) == NULL) {
            printf("categorie n existe pas!!\n");
            return;
        }
        break;

    case 2:
        printf("\n");
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("\t\t|                                    MODIFICATION DU NOM DU LIVRE                                            |\n"); 
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("nouveau nom de livre : ");
        scanf("%s",nv_livre->nom_livre);
        if(recherche_livre_par_nom(livre_arbre,nv_livre->nom_livre) != NULL){
            printf("Livre existe!!!\n");
            return;
        }
        break;        

    case 3:
        printf("\n");
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("\t\t|                                 MODIFICATION DU DATE DE PUBLICATION                                        |\n"); 
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("nouveau date de publication : ");
        scanf("%d",&nv_livre->date);
        break;

    case 4:
        printf("\n");
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("\t\t|                                     MODIFIER L'AUTEUR DU LIVRE                                             |\n"); 
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("nouveau id auteur : ");
        scanf("%d",&nv_livre->id_auteur);
        if(recherche_auteur_by_id(aut_list,nv_livre->id_auteur) == NULL){
            printf("Auteur n existe pas!!\n");
            return;
        }
        break;

    case 5:
        printf("\n");
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("\t\t|                                   MODIFIER LA CATEGORIE DU LIVRE                                           |\n"); 
        printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
        printf("nouveau categorie : ");
        scanf("%d",&nv_livre->id_categorie);
        if(recherche_cat_by_id(cat_list,nv_livre->id_categorie) == NULL) {
            printf("categorie n existe pas!!\n");
            return;
        }
        break;

    case 6:
        return;

    default:
        printf("option invalide!!!\n");
        return;
    }

    livre_arbre = supprimer_livre_par_nom(livre_arbre,rech->nom_livre);
    livre_arbre = ajouter_noeud(nv_livre,livre_arbre);
    enregistrer_arbre_in_file(livre_arbre);
    system("pause");
    menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
}

void menu_des_livres(livre *livre_arbre,user *user_list, auteur* aut_list,categorie* cat_list) {
    decorate();
    int op;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                             GESTION DES LIVRES                                             |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                          [\033[32m1\033[31m]\033[33m Supprimer un livre                                            \033[32m|\n");
    printf("\t\t|\033[31m                                          [\033[32m2\033[31m]\033[33m Ajouter un livre                                              \033[32m|\n");
    printf("\t\t|\033[31m                                          [\033[32m3\033[31m]\033[33m Afficher les livres                                           \033[32m|\n");
    printf("\t\t|\033[31m                                          [\033[32m4\033[31m]\033[33m Rechercher un livre                                           \033[32m|\n");
    printf("\t\t|\033[31m                                          [\033[32m5\033[31m]\033[33m Modifier un livre                                             \033[32m|\n");
    printf("\t\t|\033[31m                                          [\033[32m6\033[31m]\033[33m Retourner                                                     \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);
    switch (op) {

    case 1:
        supprimer_livre_menu(livre_arbre,user_list,aut_list,cat_list);
        break;

    
    case 2:
        ajoute_livre_menu(livre_arbre,user_list,aut_list,cat_list);
        break;

    case 3:
        affiche_cat_menu(cat_list,livre_arbre,aut_list);
        break;

    case 4:
        recherche_livre_menu(livre_arbre,user_list,cat_list,aut_list);
        break;

    case 5:
        modification_livre_menu(livre_arbre,user_list,aut_list,cat_list);
        break;

    case 6:
        after_admin_login_menu(user_list,aut_list);
        break;

    default:
        printf("Option invalid\n");
        system("pause");
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
        break;
    }
    system("pause");
    menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
}

void suppresion_cat_menu(categorie *cat_list, livre* livre_arbre,user* user_list, auteur* aut_list) {
    decorate();
    int op, id;
    categorie *test;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                        GESTION DES CATEGORIE                                               |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                    [\033[32m1\033[31m]\033[33m Supprimer categorie par id                                          \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m2\033[31m]\033[33m Supprimer categorie par nom                                         \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m3\033[31m]\033[33m Retourner                                                           \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);
    decorate();
    switch (op) {
    case 1:
    
        printf("Donner id du categorie a supprimer : ");
        scanf("%d", &id);
        test = recherche_cat_by_id(cat_list,id);
        if( test == NULL){
            printf("categorie non trouvee!!\n");
            system("pause");
            suppresion_cat_menu(cat_list,livre_arbre,user_list,aut_list);
        }
        printf("si vous supprimez la categorie, tous ses livres seront egalement supprimes!!\n");
        printf("1- continuer : ");
        scanf("%d",&op);
        if (op == 1) {
            livre_arbre = supprimer_livre_by_cat(livre_arbre,id);
            cat_list = supprimer_cat_by_id(cat_list,id);
            enregistrer_arbre_in_file(livre_arbre);
            enregistrer_cat_in_file(cat_list);
            system("pause");
            printf("\ncategorie supprimer avec succee!!\n\n");
        }
        break;

    case 2:
    
        char intitule[20];
        printf("donner le nom du categorie : ");
        scanf("%s", intitule);
        test = recherche_cat_by_nom(cat_list,intitule);
        if( test == NULL){
            printf("categorie non trouvee!!\n");
            system("pause");
            suppresion_cat_menu(cat_list,livre_arbre,user_list,aut_list);
        }
        printf("si vous supprimez la categorie, tous ses livres seront egalement supprimes!!\n");
        printf("1- continuer : ");
        scanf("%d",&op);
        if (op == 1) {
            livre_arbre = supprimer_livre_by_cat(livre_arbre,test->id_categorie);
            cat_list = supprimer_cat_by_nom(cat_list,intitule);
            enregistrer_arbre_in_file(livre_arbre);
            enregistrer_cat_in_file(cat_list);
            system("pause");
            printf("\ncategorie supprimer avec succee!!\n\n");
        }
        break;
    
    case 3:
        menu_des_categorie(cat_list,livre_arbre,user_list,aut_list);
        break;

    default:
        printf("Option invalid\n");
        system("pause");
        suppresion_cat_menu(cat_list,livre_arbre,user_list,aut_list);
        break;
    }
    menu_des_categorie(cat_list,livre_arbre,user_list,aut_list);
}

void approuve_demande(file *dem_file,user *user_list, auteur *aut_list, livre* livre_arbre) {
    FILE *p;
    int op;
    if (dem_file->taille == 0 || dem_file->tete == NULL)
    {
        printf("la liste des demande est vide \n");
        system("pause");
        menu_des_emprunts(dem_file,user_list,aut_list,livre_arbre);
    }
    printf("\33[31m+-----------------------------------------------------------+\33[0m\n");
    printf("   Utilisatuer %s a commande le livre %d \n", dem_file->tete->cin, dem_file->tete->id_livre);
    printf("\33[31m+-----------------------------------------------------------+\33[0m\n");

    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                           [\033[32m1\033[31m]\033[33m Approuver la demande                                         \033[32m|\n");
    printf("\t\t|\033[31m                                           [\033[32m2\033[31m]\033[33m Refuser la demande                                           \033[32m|\n");
    printf("\t\t|\033[31m                                           [\033[32m3\033[31m]\033[33m Retourner                                                    \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);

    decorate();

    demande *rm = dem_file->tete;
    switch (op)
    {
    case 1:
        p = fopen(emprunts_nom_file, "a");
        fprintf(p, "%s\n%d\n%s\n%s\n", dem_file->tete->cin, dem_file->tete->id_livre, get_current_date(), get_current_date_plus_15_days());
        fclose(p);
        p = fopen(emprunts_online_nom_file, "a");
        fprintf(p, "%s\n%d\n%s\n%s\n", dem_file->tete->cin, dem_file->tete->id_livre, get_current_date(), get_current_date_plus_15_days());
        fclose(p);
        defiler(dem_file);
        printf("\33[31m!! la demande est online !!\33[0m\n");
        print_demande_in_file(dem_file);
        system("pause");
        decorate();
        approuve_demande(dem_file,user_list,aut_list,livre_arbre);
        break;
    case 2:
        p = fopen(demande_refuse_nom_file, "a");
        fprintf(p, "%s\n%d\n%s\n", dem_file->tete->cin, dem_file->tete->id_livre, get_current_date());
        fclose(p);
        defiler(dem_file);
        printf("\33[31m!! la demande est refusee !!\33[0m\n");
        print_demande_in_file(dem_file);
        system("pause");
        decorate();
        approuve_demande(dem_file,user_list,aut_list,livre_arbre);
    case 3:
        menu_des_emprunts(dem_file,user_list,aut_list,livre_arbre);
        break;
    default:
        printf("\n invalid option!! \n\n");
        system("pause");
        decorate();
        menu_des_emprunts(dem_file,user_list,aut_list,livre_arbre);
        break;
    }
}

void Recherche_emprunt_menu(livre* livre_arbre){
    decorate();
    int option,test;
    char date[11];

    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                          RECHERCHE EMPRUNTS                                                |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                              [\033[32m1\033[31m]\033[33m Afficher les emprunts apres une date                                      \033[32m|\n");
    printf("\t\t|\033[31m                              [\033[32m2\033[31m]\033[33m Afficher les emprunts avant une date                                      \033[32m|\n");
    printf("\t\t|\033[31m                              [\033[32m3\033[31m]\033[33m Afficher une emprunt a une date specifiee                                 \033[32m|\n");
    printf("\t\t|\033[31m                              [\033[32m4\033[31m]\033[33m Retourner                                                                 \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d",&option);
    decorate();
    switch (option) {
    case 1:
        printf("Donner une date sous form (YYYY-MM-DD) : ");
        scanf("%s",date);
        test = affiche_emp_by_time(date,livre_arbre,1);
        if(test == 0)
            printf("\33[31mil n y a aucun emprunt apres cette date!!\33[0m\n");
        break;
    
    case 2:
        printf("Donner une date sous form (YYYY-MM-DD) : ");
        scanf("%s",date);
        test = affiche_emp_by_time(date,livre_arbre,-1);
        if(test == 0)
            printf("\33[31mil n y a aucun emprunt avant cette date!!\33[0m\n");
        break;

    case 3:
        printf("Donner une date sous form (YYYY-MM-DD) : ");
        scanf("%s",date);
        test = affiche_emp_by_time(date,livre_arbre,0);
        if(test == 0)
            printf("\33[31mil n y a aucun emprunt a cette date!!\33[0m\n");
        break;

    case 4:
        break;

    default:
        printf("invalid option!!\n");
        break;
    }
}

void menu_des_emprunts(file* dem_file,user *user_list, auteur *aut_list,livre* livre_arbre)
{
    decorate();
    int op,nbr;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                    GESTION DES DEMANDE D'EMPRUNT                                           |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                 [\033[32m1\033[31m]\033[33m Consulter le nombre des demandes                                       \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m2\033[31m]\033[33m Approuver ou Refuser une demande                                       \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m3\033[31m]\033[33m Afficher la liste des emprunts                                         \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m4\033[31m]\033[33m Afficher la liste des emprunts en ligne                                \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m5\033[31m]\033[33m Afficher la liste des emprunts refuses                                 \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m6\033[31m]\033[33m Rechercher une demande d'emprunt                                       \033[32m|\n");
    printf("\t\t|\033[31m                                 [\033[32m7\033[31m]\033[33m Retourner                                                              \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);
    decorate();
    switch (op)
    {
    case 1:
        nbr = dem_file->taille;
        if(nbr == 0)
            printf("\t\tla liste des demandes est vide\n");   
        else
            printf("il y a %d demandes\n",nbr);
        break;

    case 2:
        approuve_demande(dem_file,user_list,aut_list,livre_arbre);
        break;

    case 3:
        printf("\t\t\t\t\t\t\33[31m--------- liste des emprunts ---------\33[0m\n");
        affiche_all_emprunts(emprunts_nom_file, livre_arbre);
        break;

    case 4:
        printf("\t\t\t\t\t\t\33[31m--------- liste des emprunts en ligne ---------\33[0m\n");
        affiche_all_emprunts(emprunts_online_nom_file, livre_arbre);
        break;

    case 5:
        printf("\t\t\t\t\t\t\33[31m--------- liste des demandes refusee ---------\33[0m\n");
        affiche_all_demandes(demande_refuse_nom_file, livre_arbre);
        break;

    case 6:
        Recherche_emprunt_menu(livre_arbre);
        break;

    case 7:
        after_admin_login_menu(user_list,aut_list);
        break;
    default:
        printf("invalide option \n");
        break;
    }
    system("pause");
    menu_des_emprunts(dem_file,user_list,aut_list,livre_arbre);
}

void ajoute_cat_menu(categorie *cat_list){
    categorie *tmp = cat_list;
    categorie *nv = (categorie *)malloc(sizeof(categorie));
    while(tmp->suivant)
        tmp = tmp->suivant;
    nv->id_categorie = tmp->id_categorie;
    nv->id_categorie++;
    nv->suivant = NULL;

    printf("donner le nom du categorie que vous voulez ajouter : ");
    scanf("%s", nv->intitule);

    if(recherche_cat_by_nom(cat_list,nv->intitule) != NULL){
        printf("categorie existe!!\n");
        system("pause");
        return;
    }

    printf("Categorie a ete ajouter avec succes!!\n");
    cat_list = ajouter_enfin_cat(cat_list,nv);
    enregistrer_cat_in_file(cat_list);
    system("pause");
}

void recherche_categorie(categorie *cat_list) {
    decorate();
    int op;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                         RECHERCHE CATEGORIE                                                |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                        [\033[32m1\033[31m]\033[33m Recherche par ID                                                \033[32m|\n");
    printf("\t\t|\033[31m                                        [\033[32m2\033[31m]\033[33m Recherche par nom                                               \033[32m|\n");
    printf("\t\t|\033[31m                                        [\033[32m3\033[31m]\033[33m Retourner                                                       \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);
    decorate();
    categorie *temp = cat_list;
    categorie *test;
    switch (op)
    {
    case 1:
        int id;
        printf("donner l'id du categorie a recherche :");
        scanf("%d", &id);
        test = recherche_cat_by_id(cat_list,id);
        if(test == NULL) 
            printf("categorie non trouvee!!\n");
        else{
            printf("categorie existe : \n");
            printf("id : %d\n",test->id_categorie);
            printf("nom : %s\n",test->intitule);
        }
        break;

    case 2:
        char intitule[30];
        printf("donner le nom du categorie a rechercher : ");
        scanf("%s", intitule);
        test = recherche_cat_by_nom(cat_list,intitule);
        if(test == NULL) 
            printf("categorie non trouvee!!\n");
        else{
            printf("categorie existe : \n");
            printf("id  : %d\n",test->id_categorie);
            printf("nom : %s\n",test->intitule);
        }
        break;

    case 3:
        break;

    default:
        printf("invalid \n");
        recherche_categorie(cat_list);
        break;
    }
    system("pause");
}

void menu_des_categorie(categorie *cat_list,livre* livre_arbre, user* user_list,auteur* aut_list) {
    decorate();
    int option;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                         GESTION DES CATEGORIE                                              |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                     [\033[32m1\033[31m]\033[33m Supprimer une categorie                                            \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m2\033[31m]\033[33m Ajouter une categorie                                              \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m3\033[31m]\033[33m Afficher les categorie                                             \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m4\033[31m]\033[33m Rechercher une categorie                                           \033[32m|\n");
    printf("\t\t|\033[31m                                     [\033[32m5\033[31m]\033[33m Retourner                                                          \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &option);


    switch (option) {
    case 1:
        suppresion_cat_menu(cat_list,livre_arbre,user_list,aut_list);
        break;
    
    case 2:
        ajoute_cat_menu(cat_list);
        break;
    
    case 3:
        decorate();
        printf("\t\t\t\t\t\t+------------------------------------+\n");
        printf("\t\t\t\t\t\t|              CATEGORIES            |\n");
        printf("\t\t\t\t\t\t+------------------------------------+\n");
        affiche_categories(cat_list);
        system("pause");
        break;

    case 4:
        recherche_categorie(cat_list);
        menu_des_categorie(cat_list,livre_arbre,user_list,aut_list);
        break;
    case 5:
        after_admin_login_menu(user_list,aut_list);
    default:
        printf("invalid \n");
        menu_des_categorie(cat_list,livre_arbre,user_list,aut_list);
        break;
    }
    after_admin_login_menu(user_list,aut_list);
}

void recherche_user(user *user_list, livre* livre_arbre, auteur* aut_list){
    decorate();
    int op;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                        RECHERCHE UTILISATEUR                                               |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                       [\033[32m1\033[31m]\033[33m Recherche par CIN                                                \033[32m|\n");
    printf("\t\t|\033[31m                                       [\033[32m2\033[31m]\033[33m Recherche par nom                                                \033[32m|\n");
    printf("\t\t|\033[31m                                       [\033[32m3\033[31m]\033[33m Recherche par prenom                                             \033[32m|\n");
    printf("\t\t|\033[31m                                       [\033[32m4\033[31m]\033[33m Recherche par email                                              \033[32m|\n");
    printf("\t\t|\033[31m                                       [\033[32m5\033[31m]\033[33m Retourner                                                        \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d",&op);
    decorate();
    user *rech;
    switch (op)
    {
    case 1:
        char cin[20];
        printf("Donner le cin a rechercher : ");
        scanf("%s",cin);
        rech = recherche_user_par_cin(user_list,cin);
        if(rech==NULL){
            decorate();
            printf("\33[31ml'utilisateur %s n'existe pas \33[0m\n",cin);
            system("pause");
            recherche_user(user_list,livre_arbre,aut_list);
        }
        else{
            afficher_user_infos(rech);
        }
        break;

    case 2: 
        char nom[20];
        printf("Donner le nom a rechercher  : ");
        scanf("%s",nom);
        rech = recherche_user_par_nom(user_list,nom);
        if(rech == NULL){
            decorate();
            printf("\33[31ml'utilisateur %s n'existe pas \33[0m\n",nom);
            system("pause");
            recherche_user(user_list,livre_arbre,aut_list);
        }
        while (rech!=NULL){
            afficher_user_infos(rech);
            rech = recherche_user_par_nom(rech->suivant,nom);
        }
        break;

    case 3:
        printf("Donner le prenom a rechercher :  ");
        char prenom[20];
        scanf("%s",prenom);
        rech = recherche_user_par_prenom(user_list,prenom);
        if(rech == NULL){
            decorate();
            printf("\33[31ml'utilisateur %s n'existe pas \33[0m\n",prenom);
            system("pause");
            recherche_user(user_list,livre_arbre,aut_list);
        }
        while (rech != NULL){
            afficher_user_infos(rech);
            rech = recherche_user_par_prenom(rech->suivant,prenom);
        }
        break;

    case 4:
        char email[50];
        printf("Donner Email a rechercher : ");
        scanf("%s",email);
        rech = recherche_user_by_email(user_list,email);
        if(rech == NULL){
            decorate();
            printf("\33[31ml'utilisateur %s n'existe pas \33[0m\n",email);
            system("pause");
            recherche_user(user_list,livre_arbre,aut_list);
        }
        else{
            afficher_user_infos(rech);
        }
        break;

    case 5:
        menu_users(user_list,livre_arbre,aut_list); 
        break;
    default:
        printf("Option invalid!!\n");
        system("pause");
        menu_users(user_list,livre_arbre,aut_list);
        break;
    }
}

void menu_users(user *user_list, livre* livre_arbre, auteur* aut_list){
    decorate();
    int op;
    char cin[30];
    user *u = allocation_dynamique();
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                      GESTION DES UTILISATEURS                                              |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                    [\033[32m1\033[31m]\033[33m Ajouter un utilisateur                                              \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m2\033[31m]\033[33m Supprimer un utilisateur du systeme                                 \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m3\033[31m]\033[33m Modifier les informations d'un utilisateur                          \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m4\033[31m]\033[33m Rechercher un utilisateur                                           \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m5\033[31m]\033[33m Consulter les emprunts effectues par un utilisateur                 \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m6\033[31m]\033[33m Afficher tous les utilisateurs                                      \033[32m|\n");
    printf("\t\t|\033[31m                                    [\033[32m7\033[31m]\033[33m Retourner                                                           \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d",&op);

    decorate();

    switch (op) {
    case 1:
        get_info_sign_up(u);
        if(user_sign_up(user_list,u) == 1) {
            printf("\33[33mUtilisateur a ete ajouter avec succee!!!\33[0m\n");
        }
        break;
    
    case 2:
        printf("Donner le cin de utilisateur a supprimer : ");
        scanf("%s",u->cin);
        if(recherche_user_par_cin(user_list,u->cin) != NULL){
            printf("\n\33[31mtous les demande est les emprunts de cet utilisateur serons supprimer aussi!!\33[0m\n\n");
            printf("1- continuer : ");
            scanf("%d",&op);
            decorate();
            if(op == 1) {
                supprimer_demande_by_cin(u->cin);
                supprimer_demande_refu_by_cin(u->cin);
                supprimer_emprunt_by_cin(emprunts_nom_file,u->cin);
                supprimer_emprunt_by_cin(emprunts_online_nom_file,u->cin);
                supprimer_compte(user_list,u);
                printf("\33[31mutilisateur supprimer avec succee!!\33[0m\n");
            }
        }
        else{
            decorate();
            printf("\33[31mUtilisateur non trouvee!!\33[0m\n");
        }
        break;

    case 3:
        printf("Donner le cin de user a modifier : ");
        scanf("%s",cin);
        u = recherche_user_par_cin(user_list,cin);
        if(u != NULL) {
            modification_user_menu(user_list,u);
        }
        else {
            decorate();
            printf("\33[31mutilisateur non trouvee!!\33[0m\n");
        }
        break;
    
    case 4:
        recherche_user(user_list,livre_arbre,aut_list);
        break;
    
    case 5:
        printf("Donner le cin d'utilisateur : ");
        scanf("%s",cin);
        if(recherche_emprunt(cin) == NULL) {
            decorate();
            printf("\33[31mAucun emprunt ni trouvee!!\33[0m\n");
        }
        else {
            affiche_user_emprunt(cin,livre_arbre);
        }
        break;
    case 6:
        afficher_all_users(user_list);
        break;
    case 7:
        after_admin_login_menu(user_list,aut_list);
        break;

    default:
        printf("Option invalid!!\n");
        break;
    }
    print_users_list_file(user_list);
    system("pause");
    menu_users(user_list,livre_arbre,aut_list);
}

void supprimer_auteur_menu(auteur* aut_list,livre* livre_arbre,user* user_list) {
    int op;
    char nom_aut[20], prenom_aut[20];
    printf("Donner les informations du Auteur a supprimer : \n");
    printf("Nom    : ");
    scanf("%s", nom_aut);
    printf("Prenom : ");
    scanf("%s", prenom_aut);
    auteur* rech_aut = recherche_aut(aut_list, nom_aut, prenom_aut);
    if (rech_aut == NULL) {
        decorate();
        printf("l'auteur %s %s non trouvee!!\n", nom_aut,prenom_aut);
        system("pause");
        menu_des_auteurs(aut_list,livre_arbre,user_list);
    }
    livre* rech_livre = recherche_livre_by_aut(livre_arbre,rech_aut->id_aut);
    if ( rech_livre != NULL)
    {
        printf("l'auteurs %s %s a deja ecrit des livres\n", nom_aut,prenom_aut);
        printf("1- supprimer l'auteurs et les livres qu'il a ecrit \n");
        printf("2- Retourner au menu precedants \n");
        scanf("%d", &op);
        if (op == 1)
        {
            livre_arbre = supprimer_livre_by_aut(livre_arbre,rech_aut->id_aut);
            aut_list = supprimer_aut(aut_list, nom_aut, prenom_aut);
            enregistrer_arbre_in_file(livre_arbre);
            print_aut_in_file(aut_list);
            menu_des_auteurs(aut_list,livre_arbre,user_list);
            decorate();
            printf("Auteur supprimer avec succee!!\n"); 
            system("pause");             
        }
        if (op != 2)
        {
            printf("Operation Annuler!!\n");
            system("pause");
            menu_des_auteurs(aut_list,livre_arbre,user_list);
        }
    }
    else
    {
        aut_list = supprimer_aut(aut_list, nom_aut, prenom_aut);
        print_aut_in_file(aut_list);
        decorate();
        printf("Auteur supprimer avec succee!!\n"); 
        system("pause");
        menu_des_auteurs(aut_list,livre_arbre,user_list);
    }
}

void recherche_aut_menu(auteur* aut_list,livre* livre_arbre,user* user_list) {
    decorate();
    int option,id_aut;
    char nom_aut[30],prenom_aut[30];
    auteur* tmp;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                            RECHERCHE D'AUTEURS                                             |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                           [\033[32m1\033[31m]\033[33m Recherche par ID                                             \033[32m|\n");
    printf("\t\t|\033[31m                                           [\033[32m2\033[31m]\033[33m Recherche par nom et prenom                                  \033[32m|\n");
    printf("\t\t|\033[31m                                           [\033[32m3\033[31m]\033[33m Retourner                                                    \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &option);

    decorate();

    switch (option){

    case 1:
        printf("Donner l'id de l'auteur a rechercher : ");
        scanf("%d",&id_aut);
        decorate();
        tmp = recherche_auteur_by_id(aut_list,id_aut);
        if(tmp == NULL) 
            printf("Auteur non trouvee!!\n");
        else {
            printf("-------------------------------\n");
            printf("id     : %d\n",tmp->id_aut);
            printf("nom    : %s\n",tmp->nom_aut);
            printf("Prenom : %s\n",tmp->prenom_aut);
            printf("-------------------------------\n");
        }
        break;
    
    case 2:
        printf("Donner le nom d'auteur a rechercher    : ");
        scanf("%s",nom_aut);
        printf("Donner le prenom d'auteur a rechercher : ");
        scanf("%s",prenom_aut);
        decorate();
        tmp = recherche_aut(aut_list,nom_aut,prenom_aut);
        if(tmp == NULL) 
            printf("Auteur non trouvee!!\n");
        else {
            printf("-------------------------------\n");
            printf("id     : %d\n",tmp->id_aut);
            printf("nom    : %s\n",tmp->nom_aut);
            printf("Prenom : %s\n",tmp->prenom_aut);
            printf("-------------------------------\n");
        }
        break;

    case 3:
        menu_des_auteurs(aut_list,livre_arbre,user_list);
        break;

    default:
        printf("Option invalid!!\n");
        break;
    }
    system("pause");
    recherche_aut_menu(aut_list,livre_arbre,user_list);
}

void menu_des_auteurs(auteur *aut_list,livre* livre_arbre,user* user_list){
    decorate();
    int op;
    FILE *P_aut_file;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                            GESTION DES AUTEURS                                             |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                      [\033[32m1\033[31m]\033[33m Afficher la liste des auteurs                                     \033[32m|\n");
    printf("\t\t|\033[31m                                      [\033[32m2\033[31m]\033[33m Ajouter un auteur                                                 \033[32m|\n");
    printf("\t\t|\033[31m                                      [\033[32m3\033[31m]\033[33m Supprimer un auteur                                               \033[32m|\n");
    printf("\t\t|\033[31m                                      [\033[32m4\033[31m]\033[33m Rechercher un Auteur                                              \033[32m|\n");
    printf("\t\t|\033[31m                                      [\033[32m5\033[31m]\033[33m Retourner                                                         \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);
    
    decorate();

    switch (op){

    case 1:
        affichage_auteurs(aut_list);
        system("pause");
        menu_des_auteurs(aut_list,livre_arbre,user_list);
        break;

    case 2:
        auteur *nv_aut = (auteur*)malloc(sizeof(auteur));
        printf("Donner les informations du nouveau auteurs : \n");
        printf("Nom    : ");
        scanf("%s",nv_aut->nom_aut);
        printf("Prenom : ");
        scanf("%s",nv_aut->prenom_aut);
        nv_aut->id_aut = generate_id_auteur(aut_list);
        aut_list = ajoute_auteur_enfin(aut_list,nv_aut);
        P_aut_file = fopen(auteurs_nom_file,"a+");
        fprintf(P_aut_file,"%s\n%s\n%d\n",nv_aut->nom_aut,nv_aut->prenom_aut,nv_aut->id_aut);
        fclose(P_aut_file);
        printf("Auteur a ete ajouter avec succee!!\n");
        system("pause");
        menu_des_auteurs(aut_list,livre_arbre,user_list);
        break;

    case 3:
        supprimer_auteur_menu(aut_list,livre_arbre,user_list);
        break;

    case 4:
        recherche_aut_menu(aut_list,livre_arbre,user_list);
        break;

    case 5:
        after_admin_login_menu(user_list,aut_list);
        break;

    default:
        printf("Option invalid  \n");
        menu_des_auteurs(aut_list,livre_arbre,user_list);
        break;
    }
}

void after_admin_login_menu(user *user_list, auteur* aut_list) {
    decorate();
    int op;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                ESPACE ADMIN                                                |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                                            [\033[32m1\033[31m]\033[33m Gestion des livres                                          \033[32m|\n");
    printf("\t\t|\033[31m                                            [\033[32m2\033[31m]\033[33m Gestion des categorie                                       \033[32m|\n");
    printf("\t\t|\033[31m                                            [\033[32m3\033[31m]\033[33m Gestion des emprunts                                        \033[32m|\n");
    printf("\t\t|\033[31m                                            [\033[32m4\033[31m]\033[33m Gestion des utilisateurs                                    \033[32m|\n");
    printf("\t\t|\033[31m                                            [\033[32m5\033[31m]\033[33m Gestion des auteurs                                         \033[32m|\n");
    printf("\t\t|\033[31m                                            [\033[32m6\033[31m]\033[33m Retourner                                                   \033[32m|\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m");
    printf("Choisir une option : ");
    scanf("%d", &op);

    file *dem_file = (file*)malloc(sizeof(file));
    if(!dem_file)
        return;
    init_file_demande(dem_file);
    remplir_file(dem_file);

    categorie *cat_list;
    cat_list = init_cat_list(cat_list);
    cat_list = remplir_cat_list(cat_list);

    livre *livre_arbre = NULL;
    livre_arbre = remplir_livre_arbre(livre_arbre);

    switch (op){

    case 1:
        menu_des_livres(livre_arbre,user_list,aut_list,cat_list);
        break;

    case 2:
        menu_des_categorie(cat_list,livre_arbre,user_list,aut_list);
        break;

    case 3:
        menu_des_emprunts(dem_file,user_list,aut_list,livre_arbre);        
        break;

    case 4:
        menu_users(user_list,livre_arbre,aut_list);
        break;

    case 5:
        menu_des_auteurs(aut_list,livre_arbre,user_list);
        break;

    case 6:
        main_menu(user_list,livre_arbre);
        break;
    default:
        printf("invalid option!!\n");
        after_admin_login_menu(user_list,aut_list);
        break;
    }
    system("pause");
}

void after_user_login_menu(user *user_list, user *u, auteur *aut_list,categorie* cat_list, livre* livre_arbre) {
    decorate();
    int option;
    printf("\n");
    printf("\033[32m");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                              MENU APRES LA CONNEXION DE L'UTILISATEUR                                      |\n"); 
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t|\033[31m                  [\033[32m1\033[31m]\033[33m Afficher les livres par categories                     _ _                           \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m2\033[31m]\033[33m Afficher mes informations                         .-. |C| |                          \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m3\033[31m]\033[33m Modifier mes informations                         |H|_|S| |                          \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m4\033[31m]\033[33m Rechercher un livre                               |T| |S| |<\\                        \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m5\033[31m]\033[33m Gestion de mes demande d'emprunt                  |M| |.|.| \\\\                       \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m6\033[31m]\033[33m Afficher historique de mes emprunts               |L|C|#| |  \\\\                      \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m7\033[31m]\033[33m Supprimer mon compte                              | |C|+|+|   \\>                     \033[32m |\n");
    printf("\t\t|\033[31m                  [\033[32m8\033[31m]\033[33m Retour                                            \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"\"                 \033[32m |\n");
    printf("\t\t|                                                                                                            |\n");
    printf("\t\t+------------------------------------------------------------------------------------------------------------+\n");
    printf("\033[0m\n");
    printf("\nChoisir une option : ");
    scanf("%d", &option);
    decorate();
    livre *tmp_livre = livre_arbre;
    switch (option)
    {

    case 1:
        affiche_cat_menu(cat_list,tmp_livre,aut_list);
        break;
    
    case 2:
        printf("\t\t\t\t\033[1;33m=========================== voici Tes Information : =======================\33[0m\n");
        afficher_user_infos(u);
        system("pause");
        break;
    
    case 3:
        modification_user_menu(user_list,u);
        break;

    case 4:
        recherche_livre_menu(livre_arbre,user_list,cat_list,aut_list);
        break;

    case 5:
        gestion_mes_demande(livre_arbre,cat_list,user_list,u,aut_list);
        break;
    
    case 6:
        affiche_user_emprunt(u->cin,livre_arbre);
        break;

    case 7:
        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
        printf("\t\t|                                      COMPTE SUPPRIMER AVEC SUCCEE                                         |\n"); 
        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
        supprimer_demande_by_cin(u->cin);
        supprimer_demande_refu_by_cin(u->cin);
        supprimer_emprunt_by_cin(emprunts_nom_file,u->cin);
        supprimer_emprunt_by_cin(emprunts_online_nom_file,u->cin);
        user_list = supprimer_compte(user_list,u);
        print_list_in_file_user(user_list);

    case 8: 
        main_menu(user_list,livre_arbre);
        break;

    default:
        printf("\noption invalid!!!\n");
        break;
    }
    after_user_login_menu(user_list,u,aut_list,cat_list,livre_arbre);
}

void main_menu(user *user_list, livre *livre_arbre) {
    int option = 1;
    user *u = allocation_dynamique();
    auteur *aut_list = init_auteurs_list(aut_list);
    aut_list = remplir_auteurs_list(aut_list);
    categorie *cat_list;
    cat_list = init_cat_list(cat_list);
    cat_list = remplir_cat_list(cat_list);
    char c;

    while (1) {
        decorate();
        printf("\n");
        printf("\033[32m");
        printf("\t\t\t+--------------------------------------------------------------------------------------------+\n");
        printf("\t\t\t|                                           MAIN MENU                                        |\n");
        printf("\t\t\t+--------------------------------------------------------------------------------------------+\n");
        printf("\t\t\t|                                                                                            |\n");
        printf("\t\t\t|                                       %s Sign Up as User%s                                 \033[32m |\n", (option == 1) ? "\033[32m[1]" : "\033[31m[\033[32m1\033[31m]\033[33m", "\033[0m");
        printf("\t\t\t|                                       %s Log In as User%s                                  \033[32m |\n", (option == 2) ? "\033[32m[2]" : "\033[31m[\033[32m2\033[31m]\033[33m", "\033[0m");
        printf("\t\t\t|                                       %s Log In as Admin%s                                 \033[32m |\n", (option == 3) ? "\033[32m[3]" : "\033[31m[\033[32m3\033[31m]\033[33m", "\033[0m");
        printf("\t\t\t|                                       %s Quitter%s                                         \033[32m |\n", (option == 4) ? "\033[32m[4]" : "\033[31m[\033[32m4\033[31m]\033[33m", "\033[0m");
        printf("\t\t\t|                                                                                            |\n");
        printf("\t\t\t+--------------------------------------------------------------------------------------------+\n");
        printf("\033[0m"); 
        printf("\n");

        c = getch();
        switch (c) {
            case KEY_UP:
                option--;
                if (option < 1) {
                    option = 4;
                }
                break;

            case KEY_DOWN:
                option++;
                if (option > 4) {
                    option = 1;
                }
                break;

            case KEY_ENTER:
                decorate();
                switch (option) {
                    case 1:
                        printf("\033[32m");
                        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
                        printf("\t\t|                                                 SIGN UP                                                   |\n"); 
                        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
                        printf("\033[0m");
                        sign_up_menu(user_list);
                        break;

                    case 2:
                        printf("\033[32m");
                        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
                        printf("\t\t|                                                USER LOG IN                                                |\n"); 
                        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
                        printf("\033[0m");
                        user_log_in_menu(user_list, u, aut_list, cat_list, livre_arbre);
                        break;

                    case 3:
                        printf("\033[32m");
                        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
                        printf("\t\t|                                                ADMIN LOG IN                                               |\n"); 
                        printf("\t\t+-----------------------------------------------------------------------------------------------------------+\n");
                        printf("\033[0m");
                        admin_log_in_menu(user_list, u, aut_list);
                        break;

                    case 4:
                        exit(0);
                        return;
                }
                break;

            default:
                break;
        }
    }
}


int main() {

    // first interface design

    printf("\n\n");
    printf("\033[1;33m");
    printf("      |____________________________________________________| \n");
    printf("      |\033[1;37m __     __   ____   ___ \033[1;33m||\033[1;37m  ____    ____     _  __  \033[1;33m| \n");
    printf("      |\033[1;37m|  |__ |--|_| || |_|   |\033[1;33m||\033[1;37m_|**|*|__|+|+||___| ||  | \033[1;33m| \n");
    printf("      |\033[1;37m|==|^^||--| |=||=| |=*=|\033[1;33m||\033[1;37m |~~|~|  |=|=|| | |~||==| \033[1;33m| \n");
    printf("      |\033[1;37m|  |##||  | | || | |JRO|\033[1;33m||\033[1;37m-|  | |==|+|+||-|-|~||__| \033[1;33m| \n");
    printf("      ||\033[1;37m__|__||__|_|_||_|_|___|\033[1;33m||\033[1;37m_|__|_|__|_|_||_|_|_||__|_\033[1;33m|               _____                           ___. \n");
    printf("      ||_______________________||__________________________|              /  _  \\ ___.__.___.__. ____  __ _\\_ |__ \n");
    printf("      |\033[1;37m _____________________  \033[1;33m||\033[1;37m      __   __  _  __    _ \033[1;33m|             /  /_\\  <   |  <   |  |/  _ \\|  |  \\ __ \\ \n");
    printf("      |\033[1;37m|=|=|=|=|=|=|=|=|=|=|=|\033[31m __..\\/ \033[37m|  |_|  ||#||==|  / /\033[1;33m|            /    |    \\___  |\\___  (  <_> )  |  / \\_\\ \\ \n");
    printf("      |\033[1;37m| | | | | | | | | | | |\033[31m \\ \\  \\\\\033[37m|++|=|  || ||==| / / \033[1;33m|            \\____|__  / ____|/ ____|\\____/|____/|___  / \n");
    printf("      |\033[1;37m|_|_|_|_|_|_|_|_|_|_|_\033[31m/_/\\_.___\\\033[37m__|_|__||_||__|/_/__\033[1;33m|                    \\/\\/     \\/                     \\/ \n");
    printf("      |____________________ \033[31m/\\~\033[34m()\033[31m/\033[34m()\033[31m~//\\\033[33m __________________|                              ____ \n");
    printf("      |\033[1;37m __   __    _  _     \033[31m\\_  (_ .  _/\033[33m\033[1;37m _    ___     _____\033[1;33m|                             /  _ \\\n");
    printf("      |\033[1;37m|~~|_|..|__| || |_ _   \033[31m\\ //\\\\ /\033[33m  \033[1;37m|=|__|~|~|___| | | \033[1;33m|                             >  _ </\\\n");
    printf("      |\033[1;37m|--|+|^^|==|1||2| | |\033[31m__\033[32m/\\\033[33m \033[1;31m__\033[1;33m \033[32m/\\\033[33m\033[31m__\033[33m\033[1;37m| |==|x|x|+|+|=|=|=\033[1;33m|                            /  <_\\ \\/\n");
    printf("      |\033[1;37m|__|_|__|__|_||_|_| \033[31m/  \033[32m\\ \\  / /\033[31m  \\\033[33m\033[1;37m_|__|_|_|_|_|_|_|_\033[1;33m|                            \\_____\\ \\\n");
    printf("      |_________________ _\033[31m/    \033[32m\\/\\/\\/\033[31m    \\\033[33m_ _______________|                                   \\/\n");
    printf("      |\033[1;37m _____   _   __  \033[1;33m|\033[31m/      \033[32m\\\033[34m..\033[32m/\033[31m      \\\033[33m|\033[1;37m  __   __   ___\033[1;33m|             ____  __.__           .__          _____\n");
    printf("      |\033[1;37m|_____|_| |_|##|_\033[1;33m|\033[31m|   |   \033[32m\\/\033[31m __|   |\033[33m|\033[1;37m_|==|_|++|_|-||\033[1;33m|            |    |/ _|  |__ _____  |  | _____ _/ ____\\\n");
    printf("      |\033[1;37m|______||=|#|--| \033[1;33m|\033[31m\\   \\   o    /   /\033[33m| \033[1;37m|  |~|  | | ||\033[1;33m|            |      < |  |  \\\\__  \\ |  | \\__  \\   __\\ \n");
    printf("      |\033[1;37m|______||_|_|__|_\033[1;33m|_\033[31m\\   \\  o   /   /\033[33m_|\033[1;37m_|__|_|__|_|_||\033[1;33m|            |    |  \\|   Y  \\/ __ \\|  |__/ __ \\|  |\n");
    printf("      |____________________\033[31m\\___\\____/___/\033[33m___________ ______|            |____|__ \\___|  (____  /____(____  /__|\n");
    printf("      |\033[1;37m__    _  \033[1;33m/    ________     ______           /|\033[1;37m _ _ _\033[1;33m|                    \\/    \\/     \\/          \\/\n");
    printf("      |\033[1;37m\\ \\  |=|\033[1;33m/   //    /| //   /  /  / |        / |\033[1;37m| | | \033[1;33m|   \n");
    printf("      |\033[1;37m \\/\\ |*\033[1;33m/  .//____//.//   /__/__/ (_)      /  |\033[1;37m|=|=|=\033[1;33m|   \n");
    printf("    __|\033[1;37m  \\/\\|\033[1;33m/   /(____|/ //                    /  /|\033[1;37m|~|~|~\033[1;33m|__ \n");
    printf("      |___\033[1;37m\\\033[1;33m_/   /________//   ________         /  / |\033[1;37m|_|_|_\033[1;33m|  \n\n"); 
    system("pause");

    // progress bar 

    progressAnimation();

    // charger les structure

    user *user_list;
    user_list = init_user_list(user_list);
    user_list = remplir_user(user_list);
    livre *livre_arbre  = creer_noeud_livre(livre_arbre);
    livre_arbre = remplir_livre_arbre(livre_arbre);
    supprimer_emp_online_depasser();
    main_menu(user_list,livre_arbre);
    return 0; 
}