
# Tutorat de fermeture

<br>

### **<p style="text-align: center;">  COUCHE OSI ( 2 premiere couche couverte par 815)</p>**

| Physique  |  Couche marétiel |  
|---|---|
| MAC (liaison de données)  |transfert de données entre deux points, gère l'accès au canal <p>**CSMA CD** : ecoute le fil savoir si qqc se passe dessus, si rien transmet le message </p> **CSMA CA**: si ya rien on attend puis on envoie le message au complet, pendant transmission, aucun moyen de savoir si qqn lutilise.  ), ACK,  
| réseau  |  Routage de packet, gere adresse reseaux  **TCP** : garantie service, en ordr, mode connecte  <p>**UDP**: connecte ou broadcast  Aps : indirectement connect/ car on c parle a un noeud =≥ on sen sert pour definir des parametre dun noeud dans un structure. On end point : electronique, Application profile : parametre comment fonctionner **(voir procedural)**   |
|  Transport | Acheminage message  | 
| Session  |  etablie un session  |
| Présentation  |   presente linformation pour etre compativle avec la couche application|
|  Application | programmes (zdo, zigbee)  | 
![Screen Shot 2018-02-16 at 5.41.32 PM](https://i.imgur.com/ooLRxRz.png)
<br>
<br>

### **Les Websocket OU se retrouvent-elles ?**

websocket = > application. 
**pourquoi les utilise-t-on** : les 2 peuvent se parler directement.

<br>
<br>

## **Réseau embarqué => Réseau Zigbee => Réseau Ethernet**
<br>
<br>

### **<p style="text-align: center;">  Réseau embarquée</p>**

||Coordinateur|Routeur|End device|
|---|---|---|---|
|**Fonctionalité**|-Choisi le canal et choisi le PANID du reseau  <br>-Authorise les autres noeud <br>- Route les messages   | -routes les messages <br>-Authorise les autres noeud <br>- choisi le PANID      |  -Envoie et recois de l'information<br>- peut seulement envoyer au parent     |
|**sleep**| oui        |      oui  |   non   |

<br><br>
**beacon** utiliser pour identifier les reseaux, comme un sys_id : dedans : caracteristique des bande passantes. est un broadcast contenant tout linfo et sur une base de temps : toute les ___ sec


|Coordinateur||Rooteur|
|---|---|---|---|
|   |------------Beacon------------>>|    |
|   |<<------------?data----------|    |
|   |--------------Data---------->>|    |
|   |<<--------------ACK----------|    |

<br>
<br>

**Réseau Zigbee** 

* mode API
* mode Transparent
* Type noeud (voir embarquée + proc)
* Type de routage
    * **ad-hoc** (broadcast demande :qui connais le noeud x?, tout le monde ramene le chemin et construit sa table) 
    * **many-to-one**(un seul noeud = arrivé (qui broadcast), tout le monde envoie son info a une seul adresse/module : celle qui ma envoyer le broadcast car il arrive de la source)
    * **Source routing**(dans la paquet, on indique le chemin du paquet avant le data : chaque noeud enleve son nom dans le chemin donc le premier paquet = la destination et la 2e = la post-destination)

## Tableau de fonction

void myfunction1(){...}
void myfunction2(){...}

    (void* myArray[1])()={myfunction1,myfunction2}                   
  

# Procedural 1


### 1) 
### À quel protocole est-ce que les couches physiques et MAC utilisées par les modules XBee appartiennent? : 
au standard 208.15.4
### NC -> NR : `noyau coordinateur - noyau rooter`
* **Beacon** : 
    * (information sur le canal et le PAN id. Si nest pas present le coordinateur envoie cette info de temps a autre pour que le noeud prenne connaissance qu'il y ait un réseau)
    * Avec beacon : tous sont synchronisant sur une tranche de temps spécifique : permet de cadencer les instructions ( au genre 100ms)
* **Data request**:
* **Ack**:
* **Data**:
* **Ack**:

On peut aussi faire sont propre protocole

### CSMA/CD `collision detection` : pour réseau filaire car on ne peux pas ecouter pendant quon parle en wifi

**Fonctionnement**
* Message
*  At = 1 ( compteur essaie)
*  écoute occupé ? si oui on loop ^
*  Transmission
*  CD(colision detection)? 
    *  si oui 
        *  incremente att
        *  si max essaie = fin erreur
        *  sinon attendre au hasard et recommencer après message
    *  si non
        *  fin transmission
        *  si non -> on repart après le écoute occupé
        *  si oui fin erreur

### CSMA/CA : `colision avoid`
**Fonctionnement**

*not slotted*
* AT =0
* Delai
* Disponible ? CCA? 
    * oui -> transmis
    * non ->ATT +  
        * ATT > MAX?
            * oui - echec
            * non - >reviens avant le Delai

*slotted*

* AT =0, CW = 2
* Battery life
    * si ? cw(2,minBE)
    * sinon 

## Pourquoi un mécanisme d’accès au médium comme le CSMA/CD ne peut pas être utilisé ici? Donner un exemple.

see hidden node problem: c ca notre probleme
![hidden node](hiddennode.png)

## d. Qu’est-ce que la segmentation temporelle (Slotted CSMA/CA) ? Quel est son avantage?
on peut envoyer un message dans la CFP de priorite superieur

## e. Est-ce que le CSMA/CA est utilisé sur tous les types d’échange

## 4- Mécanisme d’adressage :
### a. Comment fonctionne le mécanisme d’adressage utilisé avec les modules X-Bee ?
il y a une adresse de 64 bits qui existe (MAC) 
* 3 premier octets = fabricant = standard
* 
### b. Comment est établi le parcours des paquets dans le réseau?
contien la liste de tous les nom en 16 bits = leur surnom. On etablie a partir des tables comment se rendre qqp. lorsqui ne connait pas les tables il fait du broadcast . de base coordonateur = 00000. donc au depart on connait notre adresse et le coordonateur. Entre les noeud et les coordinateurs va se creer une 'map' qui va decouvrir tout le reseau.

rooteur = 12 places dans sa table de chemin
coordinateur = 10 places dans sa table de chemin
### c. Quels sont les algorithmes de routage présents dans les réseaux ZigBee ?
si un chemin ne recois pas un akn apres un certain temps(<le temps max du chemin) (lors dun envoie de message), il continue denvoyer le message (2x). lorsqu'un noeud source -> coordinateur, il doit recevoir un ack hardware . Lors d'un envoie de message, la source le fais 4x donc le worst case scenario d'un coordinateur à rooteur = 4x le temps d'un message. calcul a prendre en compte pour le decompte du akn 
### d. Comment sont-ils mis en oeuvre et activés avec les modules X-Bee ?
3 routages differents


## 5- Dans le contexte de l’adressage au niveau de l’application (APS) :
### a. Préciser les notions suivantes : « EndPoint », « Cluster ID » et « Application Profile ».
* EndPoint : les differents capteur/modules d'un noeud
* Cluster ID : les capteurs/fonctionnalité d'un noeud , donc a chacun (lumiere = on/off, capteur = mouvement détecté)
* Application Profile : actions venant d'un cluster ID : si capteur detecte mouvement = alume led 1 P.35 livre

### b. Donner un exemple de l’utilisation de ces différents champs pour une classe  d’applications donnée.
 |1- Physique||
 |---|---|
 |2- Mac    | 802.15.4 | 
 |3- Reseau|routage|
 |4- Transport|tcp/udp|
 |5- Session||
 |6- Présentation||
 |7- Application||
 

 
### c. Quelle est la couche de protocole spécifique à ZigBee qui permet cette valeur ajoutée à l’adressage ?
APS

### d. Existe-t-elle dans 802.15.4 ?

