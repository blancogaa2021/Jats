Bayesian A/B test
=================

Met de Bayesiaanse A/B test kan men bewijs kwantificeren voor de hypothese dat een interventie of behandeling een positief, negatief of geen effect heeft. 


### Input
---------

#### Data
De ingevoerde data moet de volgende elementen bevatten:
- Aantal successen in groep 1 (controle conditie)
- Aantal trials in groep 1 (controle conditie)
- Aantal successen in groep 2 (experimentele conditie)
- Aantal trials in groep 2 (experimentele conditie 

1. Elk van deze elementen moet een geel getal zijn.
2. Er kan ook een cumulatieve reeks van successen/trials worden ingevoerd. 

#### Bayes Factor
- BF<sub>10</sub>: Als je deze optie selecteert, geeft de Bayes factor bewijs voor de alternatieve hypothese ten opzichte van de nulhypothese. Dit is de standaardoptie. 
- BF<sub>01</sub>: Als je deze optie selecteert, geeft de Bayes factor bewijs voor de nulhypothese ten opzichte van de alternatieve hypothese. Dit is gelijk aan 1/BF<sub>10</sub>.
- Log(BF<sub>10</sub>) : Als je deze optie selecteert, wordt het natuurlijke logaritme <sub>10</sub>, BF<sub>m</sub>, BF<sub>Inclusion</sub>, BF<sub>10, U</sub> weergegeven in de output.


#### Grafieken
  - Prior en posterior: Geeft de prior en posterior weer in de gewenste grootheid. De volgende grootheden kunnen worden weergegeven.
    - Log odds Ratio
    - Odds Ratio
    - Relatief risico: de ratio van de onderliggende succeskansen in de experimentele- en controleconditie.
	- Absoluut risico: het verchil tussen de succeskansen in de experimentele- en controleconditie. 
    - p1&p2: de marginale posteriors van de latente succeskansen in de experimentele- en controleconditie.
    - p1
    - p2

	(Aanvullend aan deze opties worden de mediaan en het centrale geloofwaardigheidsinterval van de posterior ook weergegeven in de grafiek.)
  - Sequentiële analyse: Geeft de ontwikkeling van de posterior terwijl de data binnenkomt. De kanswielen geven de prior en posterior kansen op de hypothese weer.
  - Bayes Factor robuustheid check: Geeft een analyse van de gevoeligheid voor de prior.  
  - Prior: Maakt een grafiek van de prior verdelingen van de parameters. Beschikbare grootheden zijn hetzelfde als voor de prior en posterior grafiek.


#### Normale prior op Log Odds Ratio
Staat toe dat een gemiddelde an standaardafwijking van de normale prior voor test-relevante log odds ratio wordt gespecificeerd. 


#### Beschrijvende statistieken
Weergeef tabel met beschrijvende statistieken: aantallen en proporties van de twee groepen.

#### Volgorde
Vergelijkt elk model met het geselecteerde model.
  - Vergelijk met het beste model.
  - Vergelijk met het nulmodel.


### Geavanceerde opties 
--------------------

#### Prior kan op model 
Specificeert de prior kansen voor de vier hypothesen: 
  - Log odds ratio = 0 (H0): Zegt dat de de succeskans gelijk is (geen effect).
  - Log odds ratio > 0 (H+): Zegt dat de succeskans groter is in de experimentele dan in de controleconditie.
  - Log odds ratio < 0 (H-): Zegt dat de succeskans groter is in de controle dan in de experimentele conditie.
  - Log odds ratio < 0 (H1): Zegt dat de succeskans verschilt tussen condities, maar zegt niet in welke conditie deze groter is.

#### Steekproefen nemen 
Bepaalt het aantal "importance samples" om de log marginale likelihood voor (H+) en (H-) te verkrijgen. Het bepaalt ook het aantal steekproeven van de posterior.

#### Reproduceerbaarheid
- Gebruik seed: Geeft de mogelijkheid een seed te gebruiken voor je analyse. Een seed gebruiken, zorgt ervoor dat willekeurige processen geen invloed hebben op een analyse.

### Uitvoer
----------

#### Model Vergelijking
  - Modellen: Hypothesen. 
  - P(M): Prior model kansen. 
  - P(M|data): Posterior kansen van de modellen.
  - BFM: Posterior model kansen. 
  - BF10 (of BF01): Bayes factor.

#### Beschrijvende statistieken
  - Groepen
  - Aantallen: successen in elke groep
  - Totaal: Steekproefgrootte van de groepen
  - Proportie


### Referenties
--------------
  - Kass R. E. and Vaidyanathan S. K. (1992). *Approximate Bayes Factors and Orthogonal Parameters, with Application to Testing Equality of Two Binomial Proportions*. Journal of the Royal Statistical Society, Series B, 54, 129-144.
  - Gronau Q.F., Raj K.N. A., Wagenmakers E. J. (2019). *Informed Bayesian Inference for the A/B Test*. arXiv preprint arXiv:1905.02068.


### R packages
--------------
  - abtest
