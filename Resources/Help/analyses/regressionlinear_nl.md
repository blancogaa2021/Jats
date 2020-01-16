Lineaire Regressie
==========================

Met een lineaire regressie kan een lineaire relatie tussen een of meer verklarende pariabele(n) (predictoren) en een continue afhankelijke (respons) variabele worden geevalueerd.

### Assumpties
---
- Continue response variabele.
- Lineariteit en additiviteit: De responsvariabele is lineair gerelateerd aan alle predictoren en de effecten van de predictoren zijn additief.
- Onafhankelijkheid van de residuen: De residuen zijn niet gecorrelleerd met elkaar
- Homoskedastischiteit: De fout variantie van elke predictor is constant over alle waarden van de predictor. 
- Normaliteit van residuen: De residuen zijn normaal verdeeld met een gemiddelde van 0.

### Invoer
---

#### Toekennningsveld
- Afhankelijk: Afhankelijke (respons) variabele.
- Methode: Specificeer de volgorde waarin de predictoren aan het model worden toegevoegd (i.e., hierarchische regressie analyse). Een blok van een of meer predictor(en) representeert een stap in de hierarchie. 
		*Let op*: De huidige versie an JASP staat niet meer dan een blok toe.
    - Toevoegen: Alle predictoren worden tegelijk toegevoegd aan het model (forced entry).
	- Achterwaards: Alle predictoren worden tegelijk toegevoegd, en daarna sequuentieel verwijderd gebaseerd op het criterium in "criterium stapsgewijze methode".
	- Voorwaarts: Predictoren worden sequentieel toegevoegd op basis van het criterium gespecificeerd in "criterium stapsgewijze methode". 
	- Stapsgewijs: Predictoren worden sequentieel toegevoegd op basis van het criterium gespecificeerd in "criterium stapsgewijze methode"; na elke stap wordt de minst nuttige predictor verwijderd. 
- Covariaten: Predictor variabele(n).
- WLS gewichten: De gewichten die worden gebruikt voor de laagste-kwadratenregressie.

### Model
- Neem intercept mee:
  - Neem het intercept mee in het model.
- Componenten en model termen:
	- Componenten: Alle onafhankelijke variabelen die in het model worden meegenomen.
	- Model termen: De onafhankelijke variabelen in het model. De standaardoptie is om de hoofdeffecten van de gespecificeerde onafhankelijke variabelen in het model te betrekken. Vink meerdere variabelen aan om interacties mee te nemen (bijv., door de cntl/cmd knop op je toetsenbord ingedrukt te houden terwijl je klikt, en sleep de variabelen naar het `Model Termen` veld.
	- Voeg toe aan nul model: De onafhankelijke variabelen in het model kunnen ook aan het nulmodel worden toegevoegd. 

### Statistieken
- Regressiecoefficiënten:
	- Schattingen: Ongestandaardiseerde en gestandaardiseerde schattingen van de coëfficiënten, standaardafwijkingen, t-waarden en de corresponderende p-waarden. 
	- Fan `...` bootstraps: Als je deze optie selecteert wordt ge-bootsttrapte schatting toegepast. De standaardoptie voor het aantal replicaties is 1000. Dit kan tot behoeven worden aangepast.
  - Betrouwbaarheidsintervallen: Door deze optie te selecteren worden betrouwbaarheidsintervallen voor het geschatte gemiddelde verschil toegevoegd. De standaardoptie is 95%. Dit kan naar behoeven worden aangepast. 
  - Covariantie matrix: geef de covariantie matrix van de predictoren per model weer.
  - Vovk-Selke maximum p-ratio: De grens 1/(-e p log(p)) wordt afgeleid van de vorm van de verdeling van de p-waardes. Onder de nulhypothese (H<sub>0</sub>) is het uniform (0,1) en onder de alternatieve hypothese (H<sub>1</sub>) neemt hij af in p, bijv. een beta (α, 1) verdeling waar 0 < α < 1. De Vovk-Selke MPR wordt verkregen door het vorm van α onder de alternative hypothese te kiezen zodat de p-waarde maximaal diagnostisch is. De waarde is dan de ratio van de dichtheid op punt p onder H<sub>0</sub> en H<sub>1</sub>. Als de tweezijdige p-waarde bijvoorbeeld .05 is, is de Vovk-Sellke MPR 2.46. Dit geeft aan dat deze p-waarde maximaal 2.46 zo waarschijnlijk is onder H1 dan onder H<sub>0</sub>. Meer informatie hier: href="https://jasp-stats.org/2017/06/12/mysterious-vs-mpr/">blogpost</a>.
  - Model fit: Aparte ANOVA tabel voor elk model (bijv., elke stap in achterwaartse, voorwaartse en stapsgewijze regressie).
  - R-Kwadraat verschil: Het verschil in R-Kwadraat tussen verschillende stappen in achterwaartse, voorwaartse en stapsgewijze regressie, met corresponderende significantie toetsen (bijv., F-waarde, df1, df2, p-waarde).
  - Beschrijvende statistieken: Steekproefgrootte, gemiddelde standaardafwijking en standaardfout van het gemiddelde.
  - Part en partiële correlaties: De semipartiële en partiële correlaties.
  - Collineariteits diagnostieken: Collineariteits statistieken, eigenwaarden, conditie indicaties en proporties variantie.

- Residuen: 
  - Statistieken: Geef beschrijvende statistieken over de residuen en voorspelde waarden.
  - Durbin-Watson: Durbin-Watson statistiek om autocorrelatie van de residuen te toetsen.
  - Gevalsgewijze diagnostiek: Gevalsgewijze en samengevatte diagnostieken voor de residuen.
	- Gestandaardiseerd residu > 3: Uitschieters buiten x standaardafwijkingen: Geef diagnostieken weer voor gevallen waar de absolute waarde voor het gestandaardiseerde residu groter is dan x; de standaardoptie is x=3.
    - Cook's afstand > 1: Geef diagnostieken weer voor gevallen waar de waarde van Cook's afstand groter is dan x; de standaardotie is x=1.
    - Alle gevallen: Geef diagnostieken voor alle gevallen weer.


### Specificatie van Methode
- Stapsgewijze methode criteria: 
  - Gebruik p-waarde: Gebruikt p-waarde als criterium voor het toevoegen en verwijderen van predictoren in achterwaartse, voorwaartse en stapsgewijze regressie.
	- Toevoegen: Voeg predictor toe als p-waarde van regressiecoëfficiënt < x. Standaard is x=0.05. 
    - Verwijderen: Verwijder predictor als p-waarde van regressiecoëfficiënt > x. Standaard is x=0.1. 
  - Gebruik F waarde: Gebruik F-waarde als criterium voor het verwijderen en toevoegen van predictoren.
    - toevoegen: Voeg predictor toe als de F waarde van de regressiecoëfficiënt > x. standaard is x=3.84.
    - Verwijderen: Verwijder predictor als de F waarde van de regressiecoëfficiënt < x. Standaard is x=2.71.

### Grafieken

- Residu grafieken: Als de assumpties van het lineaire regressiemodel houdbaar zijn moeten de residuen aselect rond een horizontale lijn liggen. Elk systematisch patroon of clustering van residuen wijst op een schending.
  - Residuen vs afhankelijk: Scatterplot van de waarden van de residuen tegen de afhankelijke variabele.
  - Residuen vs covariaten: Scatterplot van de waarden van de residuen tegen de predictor variabele(n).  
  - Residuen vs histogram: Histogram van de waarden van de residuen.
    - Gestandaardiseerde residuen: Gebruikt de gestandaardiseerde in plaats van de absolute residuen. 
  - Q-Q grafiek: Gestandaardiseerde residuen: Checkt de validiteit van de verdelingsassumpties. Om precies te zijn laat de plot zien of de residuen normaal zijn verdeeld.
  - Partiële grafieken: Deze grafieken zijn scatterplots van dde residuen van 2 regressies - regressie van de afhankelijke variabele op alle predictoren, en regressie van een predictor (als afhankelijke variabele) op alle andere predictoren -- daarna worden de residuen tegen elkaar afgezet. 



### Uitvoer
---

#### Lineaire regressie

Samenvatting van het model:
- Model: Regressie model (een voor elke stap in achterwaartse, voorwaartse en stapsgewijze regressie).
- R: Meerdere correlatie coëfficiënten R.
- R kwadraat: R kwadraat waarde. De proportie van variantie dee wordt verklaard door het regressiemodel.
- Bijgestelde R kwadraat: Bijgestelde R kwadraat waarde.
- RMSE: Root-mean-square error.
- R-kwadraat verandering: verandering in R kwadraat waarde.
- F verandering: Verandering in F-waarde.
- df1: Noemer vrijheidsgrafen van verandering in F-waarde.
- df2: Teller vrijheidsgrafen van verandering in F-waarde.
- p: p-waarde Van de varandering in F-waarde.
- Durbin-Watson: Durbin-Watson statistiek.

ANOVA:
- Model: Regressie model (een voor elke stap in achterwaartse, voorwaartse en stapsgewijze regressie.
- Kwadratensom: De kwadratensom voor het regressiemodel (regressie) en het residu (Residu), en de totale kwadratensom (totaal).
- df: De vrijheidsgraden voor het regressiemodel (regressie) en het residu (Residu), en de totale vrijheidsgraden (totaal).
- Mean square: Gekwadrateerde gemiddelde voor het regressiemodel (regressie) en het residu (residu). 
- F: F-waarde.
- p: p-waarde.

Coëfficiënten
- Model: Regressiemodel (een voor elke stap in achterwaartse, voorwaarte en stapsgewijze regressie).
- Ongestandaardiseerd: Ongestandaardiseerde coefficiënten.
- Standdaardfout: De standaardfout van de regressiecoëfficiënten.
- Gestandaardiseerd: Gestandaardiseerde coefficiënten.
- T-waarde: T-waarde voor het toetsen van de nulhypothese dat de poopulatei regressiecoëfficiënt 0 is. 
- P: P-waarde.
- % BI: Het betouwbaarheidsinterval voor de ongestandaardiseerde coefficiënten. Standaard is 95%. 
  - Linker: De linkergrens van het betrouwbaarheidsinterval.
  - Rechter: De rechtergrens van het betrouwbaarheidsinterval.
- Collineariteit statistieken:
  - Tolerantie: Omgekeerde van de Variantie Inflatie Factor (VIF).
  - VIF: Variantie Inflatie Factor; grote waarden wijzen op multicollineariteit. 
  
Ge-bootstrapte Coëfficiënten.
- Model: Regressiemodel (een voor elke stap in achterwaartse, voorwaartse en stapsgewijze regressie).
- Ongestandaardiseerd: Ongestandaardiseerde coefficiënten.
- Bias: Gestadaardiseerde coëfficiënten.
- Standdardfout: De standaardfout van de regressiecoëfficiënten.
- % BI: Het ge-bootstrapte betouwbaarheidsinterval voor de ongestandaardiseerde coefficiënten. Standaard is 95%. 
  - Linker: De linkergrens van het betrouwbaarheidsinterval.
  - Rechter: De rechtergrens van het betrouwbaarheidsinterval.

Beschrijvende statistieken.
- N: Steekproefgrootte.
- Gem: Gemiddelde.
- SD: Standaardafwijking.
- SE: Standaardfout van het gemiddelde.

Part en Partiële correlaties: 
- Model: Regressie model (een voor elke stap in achterwaardse, voorwaardse en stapsgewijze regressie).
- Partieel: Partiële correlaties tussen de predictor variabelen en de afhankelijke variabele.
- Part: Semipartiële correlates tussen de predictor variabelen en de afhankelijke variabele. 


Coefficiënten
- [linker]%: Linkergrens van het gespecificeerde x% betrouwbaarheidsinterval voor de regressiecoëfficiënten. 
- [rechter]%: Rechtergrens van het gespecificeerde x% betrouwbaarheidsinterval voor de regressiecoëfficiënten. 


Coefficiënten Covariantie Matrix:
- Geeft de covariantie matrix van de coefficiënten van de predictoren voor elk regressiemodel weer (Model). 

Collineariteit Diagnostieken: 
- Geeft voor elk  regressiemodel (model) en voor elk element van de geschaalde ongecentreerde kruisproduct matrix het volgende weer:
  - Eigenwaarde.
  - Conditie index.
  - Proportie variantie voor elke term in de regressie formule.

Stapsgewijze Diagnostieken: 
  - Voor elk gemarkeerd geval (Geval nummer) geeft dit het volgende weer:
	- Gestandaardiseerd residu.
	- De waarde van de afhankelijke variabele.
	- De voorspelde waarde.
	- Residu 
	- Cook's afstand.

Residu statistieken: 
- Geeft het minimum, maximum, gemiddelde, standaardafwijking en de steekproefgrootte voor:
 - De voorspelde waarde.
 - Het residu. 
 - Gestadaardiseerde voorspelde waarde.
 - Gestandaardiseerde residu.


### Referenties
-------
- Field, A.P., Miles, J., & Field, Z. (2012). *Discovering statistics using R*. London: Sage.
- Moore, D.S., McCabe, G.P., & Craig, B.A. (2012). *Introduction to the practice of statistics (7th ed.)*. New York, NY: W.H. Freeman and Company.
- Sellke, T., Bayarri, M. J., & Berger, J. O. (2001). Calibration of *p* values for testing precise null hypotheses. *The American Statistician, 55*(1), 62-71.
- Stevens, J.P. (2009). *Applied multivariate statistics for the social sciences (5th ed.)*. New York, NY: Routledge.

### R Packages
----
- boot
- car
- ggplot2
- lmtest
- matrixStats
- stats

