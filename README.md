Sandro CHALHOUB
Paul STARCK
3MIC - E

Pour générer des executables :

gcc -o tsock tsock_v0.c
En UDP, avec potentiellement ajout de -n[] et -l[] et avec host = srv-tp04
Puit : ./tsock -p -u 9000
Source : ./tsock -s -u srv-tp04 9000

En TCP, avec éventuellement ajout de -n[] et -l[] et avec host = srv-tp04
Puit : ./tsock -p 9000
Source : ./tsock -s srv-tp04 9000

Également possible d'échanger entre notre version de tsock et l'originale, en utilisant :
tsock -p 9000
tsock -p -u 9000
tsock -s -u host 9000
tsock -s host 9000
