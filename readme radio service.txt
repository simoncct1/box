1.Placer le fichier radio.sevice dans /etc.systemd/system
2.exécuter les commandes:
	sudo systemctl daemon-reload
			- pour prendre en compte le nouveau script à démarrer au boot:
	sudo systemctl enable radio.service
			- pour activer le démarrage automatique
			
3. reboot pour tester le démarrage

Si on veut tester que le service démarre et que le script fonctionne bien
	sudo systemctl start radio.service
	
