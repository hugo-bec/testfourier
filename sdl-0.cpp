
#include <iostream>
#include <SDL.h>


#define LARGEUR_FENETRE 1200
#define HAUTEUR_FENETRE 800

using namespace std;

SDL_Window* fenetre;
SDL_Renderer* renderer;
SDL_Texture* texture_pixels;
SDL_Texture* texture_feuille;

void* tmp_pixels;
void* tmp_feuille;
int pitch_pixels;
int pitch_feuille;
Uint32* pixels;
Uint32* feuille;

struct point {
	float x;
	float y;
};
typedef struct point point;

struct aiguille {
	point* pied;
	point* tete;
	float longueur;
	float vitesse;
	float angle;
	aiguille* fils;
};
typedef struct aiguille aiguille;


int initSDL();

void printf_aiguille(aiguille* a);
aiguille* generateur_bras(point* mine, int nb, int xracine, int yracine);
void afficher_bras(aiguille* tete, int epaisseur, int r, int g, int b);

void afficher_point(Uint32* grille, point* p, int epaisseur, int r, int g, int b);
void afficher_point(Uint32* grille, int x, int y, int epaisseur, int r, int g, int b);

void dda(Uint32* grille, point* p1, point* p2, int epaisseur, int r, int g, int b);
void bresenham(Uint32* grille, point* p1, point* p2);
int signe(int x);


int main(int argc, char* argv[])
{
	if (initSDL() == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}

	time_t t;
	srand((unsigned)time(&t));
	for (int i = 0; i < 50; i++) {
		cout << (float)rand()/RAND_MAX << "\n";
	}
	
	SDL_Event event;
	char quit = 0;
	int i = 10;
	
	int xs = 0, ys = 0;
	point souris = { (float)xs, (float)ys };
	point a = {LARGEUR_FENETRE/2, HAUTEUR_FENETRE/2 };
	point b = { 200, 250 };

	point mine;

	aiguille* bras1 = generateur_bras(&mine, 10, LARGEUR_FENETRE / 2, HAUTEUR_FENETRE / 2);
	printf_aiguille(bras1);
	printf_aiguille(bras1->fils);

	/*point p1, p2, p3;
	aiguille a3 = { &p2,	&p3,	60,			0.02,		0,	NULL };
	aiguille a2 = { &p1,	&p2,	130,		0.005,		0,	&a3 };
	aiguille a1 = { &a,		&p1,	100,		0.01,		0,	&a2};*/

	//afficher_bras(&a1);

	memset(feuille, 0, LARGEUR_FENETRE * HAUTEUR_FENETRE * sizeof(Uint32));	//initialiser une feuille blanche (enfin noire)

	while (!quit) {
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = 1;
					break;

				case SDL_MOUSEMOTION:
					SDL_GetMouseState(&xs, &ys);
					souris = { (float)xs, (float)ys };
					break;
			}
		}
		SDL_RenderClear(renderer);
		//memset(pixels, 0, LARGEUR_FENETRE * HAUTEUR_FENETRE * sizeof(Uint32));	//reset texture à 0 partout
		

		SDL_LockTexture(texture_pixels, NULL, &tmp_pixels, &pitch_pixels);
		SDL_LockTexture(texture_feuille, NULL, &tmp_feuille, &pitch_feuille);
		

			afficher_point(pixels, &souris, 51, 0xA0, 0x00, 0xA0);

			afficher_point(feuille, &mine, 1, 0, 255, 0);
			//bresenham(&a, &souris);
			//dda(&a, &souris);

			afficher_bras(bras1, 1, 255, 0, 0);


		SDL_UnlockTexture(texture_pixels);

		memcpy(pixels, feuille, LARGEUR_FENETRE * HAUTEUR_FENETRE * sizeof(Uint32));

		SDL_RenderCopy(renderer, texture_pixels, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	//cin.get();
	//SDL_Delay(3000);

	SDL_DestroyTexture(texture_pixels);
	SDL_DestroyWindow(fenetre);
	SDL_Quit();
	return 0;
}



void printf_aiguille(aiguille* a) {
	cout 
		<< " Pointeur:" << a
		<< " angle:" << a->angle
		<< " longueur:" << a->longueur
		<< " vitesse:" << a->vitesse
		<< " pied:" << a->pied
		<< " tete:" << a->tete
		<< " fils:" << a->fils

		<< "\n";
}


/* 
Pointeur:000001D01B40C950 angle:0 longueur:56.2181 vitesse:0.0140355  pied:000001D01B472540 tete:000001D01B471FF0 fils:000001D01B40C790
Pointeur:000001D01B40C790 angle:0 longueur:98.0773 vitesse:0.00693686 pied:0000003817EFFA68 tete:0000003817EFFA68 fils:0000000000000000
*/

aiguille* generateur_bras(point* mine, int nb, int xracine, int yracine) {
	point* p; point* pracine;
	aiguille* ag;
	aiguille* ag_suiv;
	aiguille* tete;

	if (nb > 0) {
		tete = (aiguille*)malloc(sizeof(aiguille));
		pracine = (point*)malloc(sizeof(point));
		pracine->x = xracine;
		pracine->y = yracine;

		tete->pied = pracine;
		tete->tete = mine;
		tete->fils = NULL;

		tete->longueur = ((float)rand() / RAND_MAX) * 100;
		tete->vitesse = ((float)rand() / RAND_MAX) * 0.02;
		tete->angle = 0;

		ag = tete;
		ag_suiv = NULL;
		--nb;

		while (nb > 0) {
			ag_suiv = (aiguille*)malloc(sizeof(aiguille));
			ag->fils = ag_suiv;
			ag->tete = (point*)malloc(sizeof(point));

			ag_suiv->pied = ag->tete;
			ag_suiv->tete = mine;
			ag_suiv->fils = NULL;

			ag_suiv->longueur = ((float)rand() / RAND_MAX) * 100;
			ag_suiv->vitesse = ((float)rand() / RAND_MAX) * 0.02;
			ag_suiv->angle = 0;

			ag = ag_suiv;
			--nb;
		}
	}
	else {
		tete = NULL;
	}

	return tete;
}



void afficher_bras(aiguille* tete, int epaisseur, int r, int g, int b) {
	aiguille* a = tete;
	while (a != NULL) {
		a->angle = fmod(a->angle + a->vitesse, M_PI*2);
		a->tete->x = cos(a->angle) * a->longueur + a->pied->x;
		a->tete->y = sin(a->angle) * a->longueur + a->pied->y;

		dda(pixels, a->pied, a->tete, epaisseur, r, g, b);
		a = a->fils;
	}
}



void afficher_point(Uint32* grille, point* p, int epaisseur, int r, int g, int b) {
	int x = round(p->x), y = round(p->y);
	for (size_t i = 0; i < epaisseur; i++) {
		for (size_t j = 0; j < epaisseur; j++) {
			if ((y + (j - (epaisseur / 2))) >= 0
				&& (y + (j - (epaisseur / 2))) < HAUTEUR_FENETRE
				&& (x + (i - (epaisseur / 2))) >= 0
				&& (x + (i - (epaisseur / 2))) < LARGEUR_FENETRE)
			{
				grille[(y + (j - (epaisseur / 2))) * LARGEUR_FENETRE + (x + (i - (epaisseur / 2)))] = r << 24 | g << 16 | b << 8 | 255 << 0;
			}
		}
	}
}
void afficher_point(Uint32* grille, int x, int y, int epaisseur, int r, int g, int b) {
	for (size_t i = 0; i < epaisseur; i++) {
		for (size_t j = 0; j < epaisseur; j++) {
			if (	(y + (j - (epaisseur / 2))) >= 0
				&&	(y + (j - (epaisseur / 2))) < HAUTEUR_FENETRE
				&&	(x + (i - (epaisseur / 2))) >= 0
				&&	(x + (i - (epaisseur / 2))) < LARGEUR_FENETRE)
			{
				grille[(y + (j - (epaisseur / 2))) * LARGEUR_FENETRE + (x + (i - (epaisseur / 2)))] = r << 24 | g << 16 | b << 8 | 255 << 0;
			}
		}
	}
}

void dda(Uint32* grille, point* p1, point* p2, int epaisseur, int r, int g, int b) {
	float x1 = p1->x, y1 = p1->y;
	float x2 = p2->x, y2 = p2->y;
	float dx = x2 - x1, dy = y2 - y1;
	float D = max(abs(dx), abs(dy));
	float incx = dx / D, incy = dy / D;
	float x = x1, y = y1;

	for (int i = 0; i < D; i++) {
		x += incx;
		y += incy;
		afficher_point(grille, round(x), round(y), epaisseur, r, g, b);
	}


}

void bresenham(Uint32* grille, point* p1, point* p2) {
	float x1=p1->x, y1=p1->y;
	float x2=p2->x, y2=p2->y;
	float sx = signe(x2 - x1);
	float sy = signe(y2 - y1);
	float dx = x2 - x1, dy = y2 - y1;
	float x = x1, y = y1;
	float valabs_axeconducteur = max(abs(dx), abs(dy));

	afficher_point(grille, x, y, 3, 0xFF, 0x00, 0x00);
	if (valabs_axeconducteur == abs(dx)) {
		float erry = 2*dy - dx;
		for (int i = 0; i < dx; i++) {
			x += sx;
			if (sy*erry >= 0) {
				y += sy;
				erry = -2 * dx;
			}
			erry += 2 * dy;
			afficher_point(grille, round(x), round(y), 3, 0xFF, 0x00, 0x00);
		}
	}
	else {	//if(valabs_axeconducteur == Math.abs(dy))
		float errx = 2*dx - dy;
		for (int i = 0; i < dy; i++) {
			y += sy;
			if (sx*errx >= 0) {
				x += sx;
				errx = -2 * dy;
			}
			errx += 2 * dx;
			afficher_point(grille, round(x), round(y), 3, 0xFF, 0x00, 0x00);
		}
	}
}

int signe(int x) {
	if (x > 0) { return 1; }
	else if (x < 0) { return -1; }
	else { return 0; }
}




int initSDL() {
	if (SDL_Init(SDL_INIT_TIMER) < 0)
	{
		cout << "SDL initialization timer failed. SDL Error: " << SDL_GetError();
		return EXIT_FAILURE;
	}
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		cout << "SDL initialization video failed. SDL Error: " << SDL_GetError();
		return EXIT_FAILURE;
	}

	fenetre = SDL_CreateWindow("Une fenetre SDL",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		LARGEUR_FENETRE, HAUTEUR_FENETRE,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);  //Création de la fenêtre

	if (fenetre == NULL)  //gestion des erreurs
	{
		printf("Erreur lors de la creation d'une fenetre : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	renderer = SDL_CreateRenderer(fenetre, -1, 
								SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL)//gestion des erreurs
	{
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	texture_pixels = SDL_CreateTexture(renderer,
					SDL_PIXELFORMAT_RGBA8888, 
					SDL_TEXTUREACCESS_STREAMING, 
					LARGEUR_FENETRE, HAUTEUR_FENETRE);

	texture_feuille = SDL_CreateTexture(renderer,
					SDL_PIXELFORMAT_RGBA8888,
					SDL_TEXTUREACCESS_STREAMING,
					LARGEUR_FENETRE, HAUTEUR_FENETRE);

	if (texture_pixels == NULL || texture_feuille == NULL) {
		printf("Erreur lors de la creation de la texture : %s", SDL_GetError());
		exit(EXIT_FAILURE);
	}
	SDL_LockTexture(texture_pixels, NULL, &tmp_pixels, &pitch_pixels);
	SDL_LockTexture(texture_feuille, NULL, &tmp_feuille, &pitch_feuille);
	pixels = (Uint32*)tmp_pixels;
	feuille = (Uint32*)tmp_feuille;
	SDL_UnlockTexture(texture_pixels);
	SDL_UnlockTexture(texture_feuille);


	cout << "SDL initialization succeeded!";
}





// Exécuter le programme : Ctrl+F5 ou menu Déboguer > Exécuter sans débogage
// Déboguer le programme : F5 ou menu Déboguer > Démarrer le débogage

// Astuces pour bien démarrer : 
//   1. Utilisez la fenêtre Explorateur de solutions pour ajouter des fichiers et les gérer.
//   2. Utilisez la fenêtre Team Explorer pour vous connecter au contrôle de code source.
//   3. Utilisez la fenêtre Sortie pour voir la sortie de la génération et d'autres messages.
//   4. Utilisez la fenêtre Liste d'erreurs pour voir les erreurs.
//   5. Accédez à Projet > Ajouter un nouvel élément pour créer des fichiers de code, ou à Projet > Ajouter un élément existant pour ajouter des fichiers de code existants au projet.
//   6. Pour rouvrir ce projet plus tard, accédez à Fichier > Ouvrir > Projet et sélectionnez le fichier .sln.
