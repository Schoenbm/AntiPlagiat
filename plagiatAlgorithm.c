#include<stdio.h>
#include<stdlib.h>
#include <sys/stat.h>
#include <string.h>

struct alignement
{
  char * x;
  char * y;
};


/* =============================================================== */
char * readtextfile(char * filename)
  /* Retourne le contenu du fichier texte filename */
/* =============================================================== */
{
  struct stat monstat;
  int N;
  char * text = NULL;
  FILE *fd = NULL;

  N = stat(filename, &monstat);
  if (N == -1)
  {
    fprintf(stderr, "error : bad file %s\n", filename);
    exit(0);
  }
  N = monstat.st_size;
  text = (char *)malloc(N+1);
  if (text == NULL)
  {   fprintf(stderr,"readtextfile() : malloc failed for text\n");
      exit(0);
  }
  fd = fopen(filename,"r");
  if (!fd)
  {
    fprintf(stderr, "readtextfile: can't open file %s\n", filename);
    exit(0);
  }
  
  fread(text, sizeof(char), N, fd);
  if((N>0) && (text[N-1] == '\n') ) text[N-1] = '\0';
  else text[N-1] = '\0';
  fclose(fd);
  return text;
}

/* =============================================================== */
int Imax(int a, int b)
/* Retourne  le maximum de a et b                                  */
/* =============================================================== */
{
  if (a < b) return b;
  else return a;	       
}

/* =============================================================== */
int Imin2(int a, int b)
/* Retourne  le minimum de a et b                                  */
/* =============================================================== */
{
  if (a < b) return a;
  else return b;	       
}

/* =============================================================== */
int Imin3(int a, int b, int c)
/* Retourne  le minimum de a, b et c                               */
/* =============================================================== */
{
  return Imin2(Imin2(a,b),c);
}

/* =============================================================== */
void retourne(char *c)
/* Retourner la chaîne de caractère c                              */
/* =============================================================== */
{
  char tmp;
  int m, j, i;
  m = strlen(c);
  j = m/2;
  for(i = 0; i < j; i++ ){
    tmp = c[i];
    c[i] = c[m-i-1];
    c[m-i-1] = tmp;
  }
}
/* =============================================================== */
void afficheSeparateurHorizontal(int nbcar)
/* =============================================================== */
{
  int i;
  printf("|-");
  for(i=0; i < nbcar; i++)
    printf("-");
  printf("-|-");
  for(i=0; i < nbcar; i++)
    printf("-");
  printf("-|\n");
}


/* =============================================================== */
void affiche(char* texte1, char* texte2, int nbcar)
  /* Affiche simultanément texte1 et texte 2 en positionnnant nbcar  
     caractères sur chaque ligne. */
/* =============================================================== */
{
  int i, l1, l2, l;
  
  char *t1,*t2;
  
  char out[512];
  
  l1 = strlen(texte1);
  l2 = strlen(texte2);

  t1 = (char*) malloc(sizeof(char) * (nbcar + 1));
  t2 = (char*)malloc(sizeof(char) * (nbcar + 1));

  l = Imax(l1, l2);
  afficheSeparateurHorizontal(nbcar);
  for(i = 0; i < l; i+= nbcar){
    if (i < l1) {
      strncpy(t1, &(texte1[i]), nbcar);
      t1[nbcar] = '\0';
    } else t1[0] = '\0';
    if (i < l2) {
      strncpy(t2, &(texte2[i]),nbcar);
      t2[nbcar] = '\0';
    } else t2[0] = '\0';
    
    sprintf(out, "| %c-%ds | %c-%ds |\n",'%', nbcar, '%', nbcar);
    printf(out, t1,t2);
  }
  afficheSeparateurHorizontal(nbcar);
  free(t1);
  free(t2);
}



/* =============================================================== */
void affiche2(char* texte1, char* texte2, int nbcar)
  /* idem affiche, mais avec un formattage différent
/* =============================================================== */
{

  int i, l1, l2, l;
  
  char *t1,*t2;
  
  char out[512];
  
  l1 = strlen(texte1);
  l2 = strlen(texte2);

  t1 = (char*) malloc(sizeof(char) * (nbcar + 1));
  t2 = (char*)malloc(sizeof(char) * (nbcar + 1));

  l = Imax(l1, l2);

  for(i = 0; i < l; i+= nbcar){
    if (i < l1) {
      strncpy(t1, &(texte1[i]), nbcar);
      t1[nbcar] = '\0';
    } else t1[0] = '\0';
    if (i < l2) {
      strncpy(t2, &(texte2[i]),nbcar);
      t2[nbcar] = '\0';
    } else t2[0] = '\0';
    
    sprintf(out, "x: %c-%ds \ny: %c-%ds\n",'%', nbcar, '%', nbcar);
    printf(out, t1,t2);

  }
  free(t1);
  free(t2);
}

//-------------------------------------------------- No modifications above --------


int optimal_alignement_matrix(int * tab, char * texte1, char * texte2, int row, int col)
{
	tab[0] = 0;
	printf("%d ; %d\n", row, col);
	for(int i = 1; i< row; i++){
		tab[i] = tab[i -1] + 1;
	}
	for(int j = 1; j< col; j++){
		tab[row * j] = tab[row * (j - 1)] + 1;
	}

	int up, left, diag, eq;
	for(int i = 1; i < row; i++){
		for(int j = 1; j< col; j++)
		{		
			eq = 1;
		
			if((texte1[i -1] == texte2[j -1]))
				eq = 0;

			up   = tab[i     + (j - 1) * row] + 1;
			left = tab[i - 1 +  j * row] + 1;
			diag = tab[i - 1 + (j - 1) * row] + eq;

			tab[i + row * j] = Imin3(up,left,diag);
		}
	}
	return tab[ row * col - 1];
}


//-----------------------------------------------------------------


void optimal_alignement_path(int* tab, struct alignement * al, char * text1, char * text2, int row, int col)
{
	int min;
	int i = row - 1;
	int j = col - 1;

	char * alx_cpy = (char *)malloc((row + col) * sizeof(char));
	char * aly_cpy = (char *)malloc((row + col) * sizeof(char));

	while(i > 0 || j > 0){

		strcpy(alx_cpy, al->x);
		strcpy(aly_cpy, al->y);

		if(i == 0){
			j -= 1;
			sprintf(al->x, "_%s", alx_cpy);
			sprintf(al->y, "%c%s", text2[j], aly_cpy);
			//printf("\nleft\n");
		}
		else if(j == 0){
			i -= 1;
			sprintf(al->x, "%c%s", text1[i], alx_cpy);
			sprintf(al->y, "_%s", aly_cpy);
			//printf("\nlup\n");
		}
		else{
			min = Imin3(tab[i + (j - 1) * row], tab[i - 1 + j * row], tab[i - 1 + (j - 1) * row]);

			//printf("min = %d, diag = %d, up = %d, left = %d\n",min, tab[i - 1 + (j - 1) * row], tab[i - 1 + j * row], tab[i + (j - 1) * row]);
			if( min == tab[i - 1 + (j - 1) * row]){
				i -= 1;
				j -= 1;
				sprintf(al->x, "%c%s", text1[i], alx_cpy);
				sprintf(al->y, "%c%s", text2[j], aly_cpy);
				//printf("\ndiag\n");
			}
			else if(min == tab[i - 1 + j * row]){
				i -= 1;	 
				sprintf(al->x, "%c%s", text1[i], alx_cpy);
				sprintf(al->y, "_%s", aly_cpy);
				//printf("\nup\n");
			}
			else if(min == tab[i + (j - 1) * row]){
				j -= 1;
				sprintf(al->x, "_%s", alx_cpy);
				sprintf(al->y, "%c%s", text2[j], aly_cpy);
				//printf("\nleft\n");
			}
			else
				break;
		}

		//printf("%d x : %s\n", i, al->x);
		//printf("%d y : %s\n", j, al->y);
	}

	free(alx_cpy);
	free(aly_cpy);
}

//------------------------------------------------------------------
struct alignement* compute_alignement(char* texte1, char* texte2)
{

	int l1 = strlen(texte1);
	int l2 = strlen(texte2);
	
	int l = Imax(l1,l2);
	int *tab = (int* )malloc(sizeof(int) * (l1+1) * (l2+1));

	
	// calcul de L dans tab.
	int score = optimal_alignement_matrix(tab, texte1, texte2, l1 + 1, l2 + 1);

	printf("score is : %d\n", score);
	printf("relative score is : %f%%\n", ((double)1 - (double)score / (double)l) * 100);
	/*
	for(int i = 0; i < (l1 + 1); i ++){
		printf("|");
		for(int j = 0; j < (l2 + 1); j++){
			printf(", %d" , tab[i + j * (l1+1)]);	
		}
		printf(" |\n");
	}
	*/
	struct alignement al;
	al.x = (char*)malloc(sizeof(char) * 2 * l);
	al.y = (char*)malloc(sizeof(char) * 2 * l);

	
	struct alignement * alptr = &al;
	optimal_alignement_path(tab, alptr,  texte1, texte2 , l1 + 1, l2 + 1);

	//printf("\n final :\n");
	//printf("x : %s\n", al.x);
	//printf("y : %s\n", al.y);

	return alptr;
}



/* =============================================================== */
int main(int argc, char **argv)
/* =============================================================== */
{
  char *x, *y; 
  
  if(argc < 3){
    x = "je suis grec";
	y = "je messuie le gras";
  }  
  else{
	x = readtextfile(argv[1]);
	y = readtextfile(argv[2]);;  
  }

  affiche(x, y, 50);
  
  struct alignement* alptr = compute_alignement(x,y);

  affiche(alptr->x, alptr->y, 50);

  free(x);
  free(y);
  
}
