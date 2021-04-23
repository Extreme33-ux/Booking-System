/*
 * File: projeto.c
 * Author: Miguel Goncalves   
 * IST Number: 194238
 * Description: Program that allows the management of events.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*The max lenght of the original string */
#define MAX_LEN_FRASE 500

/*The max lenght of the string that contains the description or names of participants/responsible for an event.*/
#define MAX_LEN 65

/*The max number of events at a given time */
#define MAX_EVENTOS 1000

/*The max number of people in a given event */
#define MAX_PARTICIPANTES 4


/* Definition of an "Evento" structure, which contains the information about a given one */
typedef struct evento {

	char descricao[MAX_LEN];
	int dia;
	int hora;
	int duracao;
	int sala;
	char participantes[MAX_PARTICIPANTES][MAX_LEN];
}Evento;

/* Array of "Evento". */
Evento Matrix[MAX_EVENTOS];

/*String that contains a character, which chooses which operation to perform. */
char *comando;

/*String that contains the info needed to perform a given operation. */
char *EntryData;

/*Variable that counts the numbers of events that exist at any time in the 'Matrix'.*/
int counter;

/* Declaration of functions */

void obter_comando();
void inserir_evento();
void todos_eventos();
void eventos_sala();
void apagar_evento();
void alterar_hora();
void alterar_duracao();
void alterar_sala();
void adicionar_participante();
void remover_participante();
int cmp_event(const void*a, const void*b);
int aux_cmp(const void*a, const void*b);
int hora_final(Evento b);
int inverter_dia(int x);
int check_overlap_tempo(Evento b);
int check_overlap_pessoas(Evento b, int x);

/* Reads input from stdin, and decides which operation to do according to the first character of the input.
After performing an operation, sorts two times to first put all the events on the left side of the array,then 
sorts again 'counter' events chronologically in the array. Input will still be asked until it receives an 'x'
to terminate the program. */

int main(){

	char frase[MAX_LEN_FRASE];

	fgets(frase,MAX_LEN_FRASE,stdin);
	comando = strtok(frase, " ");
	EntryData = strtok(NULL,"\n");

	while (*comando !='x'){

		switch(*comando){

			case 'a':
				inserir_evento();
				break;
			case 'l':
				todos_eventos();
				break;
			case 's':
				eventos_sala();
				break;
			case 'r':
				apagar_evento();
				break;
			case 'i':
				alterar_hora();
				break;
			case 't':
				alterar_duracao();
				break;
			case 'm':
				alterar_sala();
				break;
			case 'A':
				adicionar_participante();
				break;
			case 'R':
				remover_participante();
				break;
		}
		qsort(Matrix,MAX_EVENTOS,sizeof(*Matrix),aux_cmp);
		qsort(Matrix,counter,sizeof(*Matrix),cmp_event);
		fgets(frase,MAX_LEN_FRASE,stdin);
		comando = strtok(frase, " ");
		EntryData = strtok(NULL,"\n");
	}
	return 0;
}

/* Function that obtains the information needed to create an "evento", according to input. Then,
checks if the "evento" overlaps in time(two "events" cannot occur at the same time in the same room) 
and in persons(a participant/responsible cannot be in two events which occur at the same time in different rooms).
If it meets at least one of these requirements, an Error message will be displayed, explaining the error. Else,
the event is saved on the array 'Matrix', which stores 'evento', */

void inserir_evento(){

	int i;
	int j=0;
	Evento a;
	char* nomes = NULL;

	strcpy(a.descricao, strtok(EntryData, ":"));
	a.dia = atoi(strtok(NULL, ":"));	
	a.hora = atoi(strtok(NULL, ":"));
	a.duracao = atoi(strtok(NULL, ":"));	
	a.sala = atoi(strtok(NULL, ":"));
	while ((nomes = strtok(NULL, ":")) != NULL){
		strcpy(a.participantes[j++],nomes);
		}
	while (j< MAX_PARTICIPANTES) {
		strcpy(a.participantes[j++],"\0");
		}
	for (i=0;i<MAX_EVENTOS;i++){
		if (Matrix[i].dia==0){
			Matrix[i]=a;
			if(check_overlap_tempo(Matrix[i])==0){
				printf("Impossivel agendar evento %s. Sala%d ocupada.\n",a.descricao, a.sala);
				Matrix[i].dia = 0;
				strcpy(Matrix[i].descricao,"\0");
				return;
			}
			else if(check_overlap_pessoas(Matrix[i],0)==0){
				Matrix[i].dia = 0;
				strcpy(Matrix[i].descricao,"\0");
				return;
			}
			else if ((check_overlap_tempo(Matrix[i])==1) && (check_overlap_pessoas(Matrix[i],0)==1)){
				counter++;
				return;
			}
			break;
		}
	}
}

/* Function that prints all the 'eventos' chronologically, that are saved in the 'Matrix' */
void todos_eventos(){

	int i, j;

	for (i = 0; i < counter; i++) {
		if (Matrix[i].sala != 0) {
			printf("%s %08d %04d %d Sala%d %s\n*",Matrix[i].descricao,Matrix[i].dia, Matrix[i].hora, Matrix[i].duracao, Matrix[i].sala, Matrix[i].participantes[0]);
			for (j = 1; j < MAX_PARTICIPANTES; j++) {
				if (strcmp(Matrix[i].participantes[j],"\0")!= 0){
					printf(" %s", Matrix[i].participantes[j]);
					continue;
					}
			}
			printf("\n");
		}
	} 
}

/* Function that prints all the 'eventos' chronologically on a given room, that are saved in the 'Matrix' */
void eventos_sala(){

	int sala = atoi(EntryData);
	int i,j;

	for (i = 0; i < counter; i++) {
		if (Matrix[i].sala == sala) {
			printf("%s %08d %04d %d Sala%d %s\n*",Matrix[i].descricao,Matrix[i].dia, Matrix[i].hora, Matrix[i].duracao, Matrix[i].sala, Matrix[i].participantes[0]);
			for (j = 1; j < MAX_PARTICIPANTES; j++) {
				if (strcmp(Matrix[i].participantes[j],"\0")!= 0){
					printf(" %s", Matrix[i].participantes[j]);
					continue;
					}
			}
			printf("\n");
		}
	}
}

/* Function that allows the erasement of an 'evento'. Sets the given 'evento' day to 0 and the description to NULL.
If the given 'evento' does not exist on the 'Matrix', an Error will be displayed. */
void apagar_evento(){

	Evento a;
	int i;
	int j=0;

	strcpy(a.descricao,EntryData);

	for (i = 0; i < counter; i++) {

		if (strcmp(Matrix[i].descricao, a.descricao)==0){

			strcpy(Matrix[i].descricao, "\0");
			Matrix[i].dia = 0;
			Matrix[i].hora = 0;
			Matrix[i].duracao = 0;
			Matrix[i].sala = 0;
			while (j< MAX_PARTICIPANTES) { strcpy(Matrix[i].participantes[j++], "\0"); }
			counter--;
			return;
		}
	}
	printf("Evento %s inexistente.\n", a.descricao);
}

/* Function that allows the change of the hour at which the 'evento' starts. It checks for overlap of time and persons.
If one of them at least checks out, an Error message will be displayed. Otherwise, the 'evento' is updated in the Matrix. */
void alterar_hora(){

	Evento a;
	int i,t;

	strcpy(a.descricao,strtok(EntryData, ":"));
	a.hora = atoi(strtok(NULL, ":"));

	for (i = 0; i < counter+1; i++){
		if (strcmp(Matrix[i].descricao, a.descricao)==0){
			t = Matrix[i].hora;
			Matrix[i].hora = a.hora;

			if (check_overlap_tempo(Matrix[i]) == 0){
				printf("Impossivel agendar evento %s. Sala%d ocupada.\n",Matrix[i].descricao, Matrix[i].sala);
				Matrix[i].hora = t;
				return;
			}
			else if(check_overlap_pessoas(Matrix[i],0)==0){
				Matrix[i].hora = t;
				return;
			}

			else if ((check_overlap_tempo(Matrix[i])==1)&&(check_overlap_pessoas(Matrix[i],0)==1)){
				Matrix[i].hora = a.hora;
				return;
			}
		}
	}
	printf("Evento %s inexistente.\n", a.descricao);  
}

/* Function that allows the change of the duration of a given 'evento'. It checks for overlap of time and persons.
If one of them at least checks out, an Error message will be displayed. Otherwise, the given'evento' is updated in the Matrix. */
void alterar_duracao(){

	Evento a;
	int i,t;

	strcpy(a.descricao,strtok(EntryData, ":"));
	a.duracao = atoi(strtok(NULL, ":"));

	for (i = 0; i < counter+1; i++){
		if (strcmp(Matrix[i].descricao, a.descricao)==0){
			t = Matrix[i].duracao;
			Matrix[i].duracao = a.duracao;

			if (check_overlap_tempo(Matrix[i]) == 0){
				printf("Impossivel agendar evento %s. Sala%d ocupada.\n",Matrix[i].descricao, Matrix[i].sala);
				Matrix[i].duracao = t;
				return;
			}
			else if(check_overlap_pessoas(Matrix[i],0)==0){
				Matrix[i].duracao = t;
				return;
			}
			else if ((check_overlap_tempo(Matrix[i])==1)&&(check_overlap_pessoas(Matrix[i],0)==1)){
				Matrix[i].duracao = a.duracao;
				return;
			}

		}
	}
	printf("Evento %s inexistente.\n", a.descricao);
}
/* Function that allows the change of the room at which the 'evento' occurs. It checks for overlap of time.
If it checks out, an Error message will be displayed. Otherwise, the 'evento' is updated in the Matrix. */
void alterar_sala(){

	Evento a;
	int i,t;

	strcpy(a.descricao,strtok(EntryData, ":"));
	a.sala = atoi(strtok(NULL, ":"));

	for (i = 0; i < counter+1; i++){
		if (strcmp(Matrix[i].descricao, a.descricao)==0){
			t = Matrix[i].sala;
			Matrix[i].sala = a.sala;

			if (check_overlap_tempo(Matrix[i]) == 0){
				printf("Impossivel agendar evento %s. Sala%d ocupada.\n",Matrix[i].descricao, Matrix[i].sala);
				Matrix[i].sala = t;
				return;
			}

			else if (check_overlap_tempo(Matrix[i])==1){
				Matrix[i].sala = a.sala;
				return;
			}
		}
	}
	printf("Evento %s inexistente.\n", a.descricao);
}

/* Functions that allows the addition of a participant to the 'evento'. Checks first if there's space for the participant
(a max of 3 participants is allowed).If so, checks if there's no overlap for that participant. If it does not verify, then
the 'Evento' is updated on the 'Matrix'.Otherwise, a display Error will appear. If the participant is already on the given
'Evento', nothing happens. */
void adicionar_participante(){

	Evento a;
	int i,j;

	strcpy(a.descricao,strtok(EntryData, ":"));
	strcpy(a.participantes[0],strtok(NULL, ":"));

	for (i = 0; i < counter+1; i++){
		if (strcmp(Matrix[i].descricao, a.descricao)==0){
			for (j=0;j<MAX_PARTICIPANTES;j++){
				if (strcmp(Matrix[i].participantes[j],a.participantes[0])==0){
					return;
				}
				else if (strcmp(Matrix[i].participantes[j], "\0")==0){

					strcpy(Matrix[i].participantes[j],a.participantes[0]);

					if (check_overlap_pessoas(Matrix[i],1)==0){
						printf("Impossivel adicionar participante. Participante %s tem um evento sobreposto.\n",a.participantes[0] );
						strcpy(Matrix[i].participantes[j], "\0");
						return;
					}
					else if (check_overlap_pessoas(Matrix[i],1)==1){
						return;
					}
				}
			}
			printf("Impossivel adicionar participante. Evento %s ja tem 3 participantes.\n", a.descricao);
			return;
		}
	}
	printf("Evento %s inexistente.\n", a.descricao);
}

/* Functions that allows the removal of a participant to the 'evento'. Checks first if it's the only participant
(a minimum of 1 participant is required, asides from the responsible). If not, the list of the participants is updated, according
to the order of the participants that have been added.If it does not verify, then the 'Evento' is updated on the 'Matrix'.
Otherwise, a display Error will appear. If the participant isn't on the given 'Evento', nothing happens. */
void remover_participante(){

	Evento a;
	int i,j,k;
	int diff=0;

	strcpy(a.descricao,strtok(EntryData, ":"));
	strcpy(a.participantes[0],strtok(NULL, ":"));

	for (i = 0; i < counter+1; i++){
		if (strcmp(Matrix[i].descricao, a.descricao)==0){
			for (j=1;j<MAX_PARTICIPANTES;j++){
				if (strcmp(Matrix[i].participantes[j],a.participantes[0])==0){
					strcpy(Matrix[i].participantes[j], "\0");

					for (k=1;k<MAX_PARTICIPANTES;k++){

						diff +=strcmp(Matrix[i].participantes[k],"\0");
					}
					if (diff == 0){
						printf("Impossivel remover participante. Participante %s e o unico participante no evento %s.\n",a.participantes[0], a.descricao );
						strcpy(Matrix[i].participantes[j],a.participantes[0]);
						return;
					}

					else if(diff !=0){
							if(strcmp(Matrix[i].participantes[1],"\0")==0){
								strcpy(Matrix[i].participantes[1],Matrix[i].participantes[2]);
								strcpy(Matrix[i].participantes[2], Matrix[i].participantes[3]);
								strcpy(Matrix[i].participantes[3], "\0");
							}
							else if (strcmp(Matrix[i].participantes[2],"\0")==0){
								strcpy(Matrix[i].participantes[2], Matrix[i].participantes[3]);
								strcpy(Matrix[i].participantes[3], "\0");
							}
							else if (strcmp(Matrix[i].participantes[3],"\0")==0){
								strcpy(Matrix[i].participantes[3], "\0");
							}
						return;
						}
				}
			}
			return;
		}
		continue;
	}
	printf("Evento %s inexistente.\n", a.descricao);
}

/* AUXILIAR FUNCTIONS */

/* Function that receives an 'evento' and returns the hour at which the 'evento' ends. */
int hora_final(Evento b){ 
	int x;

	x =((b.hora/100+b.duracao/60)*100+ ((b.hora%100 +b.duracao%60)/60)*100+(b.hora%100 +b.duracao%60)%60);

	return x;
}

/* Function that receives the integer that represent the day of a given 'Evento', and returns its inverse form.
Example:29032019 becomes 20190329. */
int inverter_dia(int x){

	int dia,mes,ano;

	dia = x/1000000;
	mes = (x/10000)-(dia*100);
	ano = x%10000;


	return (ano*10000+mes*100+dia);
}
/* Function used in the first qsort that appears in the main. Allows to put all the created events to the left side of the array. */
int cmp_event(const void*a, const void*b){

	int diff;
	Evento *ev1, *ev2;

	ev1 = (Evento*) a;
	ev2 = (Evento*) b;

	diff = (inverter_dia(ev1->dia) - inverter_dia(ev2->dia));
	if (diff ==0){

		diff += ev1->hora -ev2->hora;

		if (diff ==0){

			diff +=ev1->sala - ev2->sala;
			}
	}
	

	return diff;
}

/*Function used in the second qsort that appears in the main. ALlows to order chronologically all the events created. */
int aux_cmp(const void*a, const void*b){

	int diff;
	Evento *ev1, *ev2;

	ev1 = (Evento*) a;
	ev2 = (Evento*) b;

	diff = -(inverter_dia(ev1->dia) - inverter_dia(ev2->dia));
	if (diff ==0){

		diff += ev2->hora -ev1->hora;
	}

	return diff;
}

/* Function that checks for overlap of events in terms of time and room(Two events cannot occur at the same time in the same room). */
int check_overlap_tempo(Evento b){

	int j;

	for (j=0; j<counter+1;j++){
		if (b.dia == Matrix[j].dia){
			if ((strcmp(b.descricao, Matrix[j].descricao)!=0) && (b.sala == Matrix[j].sala)){
				if (!((hora_final(b)<= Matrix[j].hora) || (hora_final(Matrix[j])<=b.hora))){
					return 0;
				}
			}
			continue;
		}
		continue;
	}

	return 1;
}

/*Function that checks for overlap of a person's event(A person cannot be in two events at the same time). */
int check_overlap_pessoas(Evento b, int x){

	int j,k,l;
	int ctr=0;
	for (j=0; j<counter+1;j++){
		if(b.dia == Matrix[j].dia){
			if ((strcmp(b.descricao, Matrix[j].descricao)!=0) && (b.sala != Matrix[j].sala)){
				if (!((hora_final(b)<= Matrix[j].hora) || (hora_final(Matrix[j])<=b.hora))){
					for (k=0;k<MAX_PARTICIPANTES;k++){
						if (b.participantes[k][0] == 0){
								break;
								}
						for (l=0; l<MAX_PARTICIPANTES; l++){
							
							if (strcmp(b.participantes[k],Matrix[j].participantes[l])==0){
								if (x==0){
									printf("Impossivel agendar evento %s. Participante %s tem um evento sobreposto.\n",b.descricao,b.participantes[k]);
									ctr++;
								}
								else if (x==1){
									ctr++;
								}
							}
						}
					}
				}
			}
		}
	}

	if (ctr==0){
		return 1;
	}
	else if (ctr!=0){
		return 0;
	}
	else{
		return 1;
	}
}
