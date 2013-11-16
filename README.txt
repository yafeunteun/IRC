IRC
===

https://github.com/yafeunteun/PSEUDO-IRC-SERVER


Composition du binôme 
----------------------

Yann FEUNTEUN
Victorien MOLLE


Description du projet
----------------------

Serveur pseudo-irc. Le but de ce serveur est de permettre l'échange de messages via internet ou via un réseau local. Le seveur implémente un certain nombre de commandes qui se rapprochent de la norme RFC-1459 sur l'IRC.


Liste des commandes qui fonctionnent
-------------------------------------

Toute les commandes sont implémentées et semblent fonctionner.
Pas de bugs connus.


Fonctionnalités spécifiques additionnelles
-------------------------------------------

Le serveur doit être configuré grâce au fichier server.conf

Nous utilisons le pattern Singleton pour la classe Server et le pattern Command pour la classe Command. 

Ceci nous permet une plus grande modularité du code. 




De plus notre classe Frame permet une bonne abstraction du format de trame des messages échangés, l'usage du code est ainsi facilité. Une personne qui ne connait pas le format d'une trame peut aisément venir ajouter des commandes au projet de la manière suivante :

- ajout d'un code commande dans le namespace CMD (voir Command.h)
- ajout d'une classe décrivant sa nouvelle commande calquée sur les autres classes qui héritent de la classe Command.
- ajout de la méthode correspondante dans la classe Server.

Le code est assez répétitif ce qui permet à une personne extérieure au projet d'avoir un nombre important d'exemples à disposition pour faire des ajouts.

De plus les Commandes étant séparées de la classe Server (utlisation du pattern command), celle-ci peut être remplacée.

