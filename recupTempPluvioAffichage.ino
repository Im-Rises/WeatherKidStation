// lecture de l'état du bouton
const int numBouton=9;
int etat_bouton=0;
bool choixAfficheurTempPluvio=false;
long tempsChangementAffich =millis();

// définition des broches du décodeur 7 segments
const int bit_A = 2;
const int bit_B = 3;
const int bit_C = 4;
const int bit_D = 5;

// définitions des broches des transistors pour chaque afficheur
const int alim_dizaine = 6;         // les dizaines
const int alim_unite = 7;           // les unites
const int alim_decimal = 8;         // les décimals

//Définition du temp d'affichage pour les afficheurs sept-segments
long tempsaffichage = millis();     // Initialise le temps pour le changement d'afficheur sept-segments

int afficheur=0;                    // La variable définis le numéro de l'afficheur à allumé (initialisé
                                    // à 0 pour les dizaines, à 1 pour l'unité et à 2 pour la décimale

//Variable temps recup données
long tempRecupDonnees =millis();



// Variable du capteur de température
const int analogPinTemperature = A0;//Affecte le pin A0 à une variable

int valCaptTemp =0;//Valeur de température mesurée par la carte Arduino

float valTemp = 0;//Valeur de température convertie en degré celsius


// Variable du capteur de température
const int analogPinPluvio = A1;//Affecte le pin A0 à une variable

int valCaptPluvio=0;//Valeur de pluvimétrie mesurée par la carte Arduino

float valPluvio = 0;//Valeur de pluvimétrie convertie en Force


void setup()
{
  // Les broches de commande des afficheurs sept-segments sont toutes des sorties
  pinMode(bit_A, OUTPUT);
  pinMode(bit_B, OUTPUT);
  pinMode(bit_C, OUTPUT);
  pinMode(bit_D, OUTPUT);
  pinMode(alim_dizaine, OUTPUT);
  pinMode(alim_unite, OUTPUT);
  pinMode(alim_decimal,OUTPUT);

  // Les broches de commande des afficheurs sept-segments sont toutes mises à l'état bas
  digitalWrite(bit_A, LOW);
  digitalWrite(bit_B, LOW);
  digitalWrite(bit_C, LOW);
  digitalWrite(bit_D, LOW);
  digitalWrite(alim_dizaine, LOW);
  digitalWrite(alim_unite, LOW);
  digitalWrite(alim_decimal, LOW);

  // Initialisation bouton
  pinMode(numBouton, INPUT);
  
  Serial.begin(9600);//Initialisation de la vitesse de communication entre l'arduino et le moniteur série
}

void loop()
{
  if((millis() - tempRecupDonnees) > 2000)
  { 
    //Lecture de la valeur lue de la température sur le pin A0
    //valCaptTemp = analogReadPerso(analogPinTemperature);
    valCaptTemp=analogRead(analogPinTemperature);
  
    //Conversion de la température lue sur le pin A0 en degré celsius
    valTemp = conversionNumeriqueTemperature(valCaptTemp);

    //Lecture de la valeur lue de la pluviométrie sur le pin A0
    //valCaptPluvio =analogReadPerso(analogPinPluvio);
    valCaptPluvio =analogRead(analogPinPluvio);
  
    //Conversion de la pluvimetrie lue sur le pin A1 en degré celsius
    valPluvio = conversionNumeriquePluviometrie(valCaptPluvio);
  
    tempRecupDonnees=millis();
  }
  
  //Recup appuit bouton
  boutonRecup();
  
  //Si un appui sur le bouton est effectué, on inverse la variable booléenne "choixAfficheurTempPluvio"
  //afin d'envoyer la température ou la pluvimétrie précéédemment récupérer et convertie.
  if(etat_bouton==1 )
  {
    choixAfficheurTempPluvio=!choixAfficheurTempPluvio;
  }
  

  if(choixAfficheurTempPluvio)
  {
    //Affichage de la température relevée avec les afficheurs sept-segments
    appelAfficheur(valTemp);
  }
  else
  {
    //Affichage de la pluviometrie relevée avec les afficheurs sept-segments
    appelAfficheur(valPluvio);
  }  
}

void boutonRecup()
{
    // lecture de l'état du bouton d'incrémentation
    etat_bouton = digitalRead(numBouton);
}

int  analogReadPerso(uint8_t  pin)  
 {
  //  Pour  les  cartes  Mega
  #if  defined(__AVR_ATmega1280__)
  //  Si  le  pin  considéré  est  de  type  Ax,  on  utilise  ça
  if  (pin  >=  54)  pin  -=  54;
  //  Pour  les  Uno  et  Nano
  #else
  if  (pin  >=  14)  pin  -=  14;
  #endif

  //  Définition  de  la  référence  de  tension
  ADMUX  |=  (1  <<  REFS0);
  //  On  sélectionne  notre  pin
  ADMUX  |=  pin  &  0x07;

  #if  defined(ADCSRB)  &&  defined(MUX5)
  //  Utilisation  de  MUX5  sur  la  Mega  pour  les  pins  au-dessus  de A7
  ADCSRB  =  (ADCSRB  &  ~(1  <<  MUX5))  |  (((pin  >>  3)  &  0x01)  << MUX5);
  #endif

  //  On  lance  la  conversion
  //sbi(ADCSRA,  ADSC);
  ADCSRA|=0b01000000;

  //  Le  bit  sera  désactivé  à  la  fin  de  la  conversion
  while(bit_is_set(ADCSRA,  ADSC));

  //  Lire  ADCL  en  premier  est  obligatoire,  sinon  l'ADC  se  bloque
  uint8_t  low  =  ADCL;

  //  Récupérer  le  résultat
  return  (ADCH  <<  8)  |  low;
}

float conversionNumeriqueTemperature(int valeurFunc)
{
  //Conversion de la température relevée en degré celsius
  return ((float)valeurFunc*5)*100/1023;
}

float conversionNumeriquePluviometrie(int valeurPluvioFunc)
{
  //Conversion de la pluviométrie relevée
  return (float)(((float)780/(float)1023)*(float)valeurPluvioFunc);
}

void appelAfficheur(double valeur)
{
  // Si le temps mesurée depuis le dernier affichage est 
  // supérieur à 6ms, on change d'afficheur sept-segment à allumer
  if((millis() - tempsaffichage) > 6)
  {    
    // Etant donné que les afficheurs sont numérotés de 0 à 2,
    // on n'a pas d'afficheur 3
    if(afficheur>2)
      afficheur=0;

    // Afficher nombre sur l'afficheur sept-segment sélectionnée avec la variable "afficheur"
    afficher_nombre(valeur);

    // Changement d'afficheur sept-segment
    afficheur++;
    
    // on met à jour le temps
    tempsaffichage = millis(); 
  }
}


// fonction permettant d'afficher un nombre
// elle affiche soit les dizaines soit les unités soit les decimals
void afficher_nombre(float nombre)
{  
  char unite = 0, dizaine = 0, decimal=0;
    
  if((char)nombre > 9)
      dizaine = nombre / 10; // on recupere les dizaines
  unite = nombre - (dizaine*10); // on recupere les unités

  decimal = (nombre-(char)nombre)*10;// on recupère les decimals
  
  //affichage sur un des afficheurs en fonction de la variable afficheur
  if(afficheur ==0)
  {
      // on affiche les dizaines
      // Pour cela on éteint les afficheurs sept-segments de l'unité et de la décimale
      digitalWrite(alim_unite, LOW);
      digitalWrite(alim_decimal,LOW);
      afficher(dizaine);
      digitalWrite(alim_dizaine, HIGH);
  }
  else if (afficheur ==1)
  {
      // on affiche les unités
      // Pour cela on éteint les afficheurs sept-segments de la dizaine et de la décimale
      digitalWrite(alim_dizaine, LOW);
      digitalWrite(alim_decimal,LOW);
      afficher(unite);
      digitalWrite(alim_unite, HIGH);
  }
  else
  {
    // on affiche les décimal
    // Pour cela on éteint les afficheurs sept-segments de la dizaine et de l'unité
      digitalWrite(alim_dizaine, LOW);
      digitalWrite(alim_unite,LOW);
      afficher(decimal);
      digitalWrite(alim_decimal, HIGH);
  }
}

// fonction écrivant sur l'afficheur allumé la valeur envoyé un paramètre de 0 à 9
void afficher(char chiffre)
{
  //La fonction envoie de l'arduino au décodeur sept-segment un message en 4 bits
  //qui va définir la valeur à afficher sur les afficheurs sept-segments allumés
  //en fonction de la valeur envoyé en paramètre dans la fonction
  digitalWrite(bit_A, LOW);
  digitalWrite(bit_B, LOW);
  digitalWrite(bit_C, LOW);
  digitalWrite(bit_D, LOW);

  if(chiffre >= 8)
  {
      digitalWrite(bit_D, HIGH);
      chiffre = chiffre - 8;
  }
  if(chiffre >= 4)
  {
      digitalWrite(bit_C, HIGH);
      chiffre = chiffre - 4;
  }
  if(chiffre >= 2)
  {
      digitalWrite(bit_B, HIGH);
      chiffre = chiffre - 2;
  }
  if(chiffre >= 1)
  {
      digitalWrite(bit_A, HIGH);
      chiffre = chiffre - 1;
  }
}
