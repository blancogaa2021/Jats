Herhaalde Metingen ANOVA
===

Met de herhaalde metingen ANOVA kan men verschillen tussen gemiddelden analyseren wanneer observaties afhankelijk zijn.

### Assumpties
- De afhankelijke variabele is normaal verdeeld voor elke groep.
- De covariaat en het experimentele effect zijn onafhankelijk.
- Er is aan de assumptie van sphericiteit voldaan. Dit houdt in dat varianties van de verschillen tussen de herhaalde meting condities gelijk zijn.

### Invoer
---

#### Invoerveld 
- Herhaaldelijke meting factoren: De binnen-subject variabele. Hier kan je de binnen-subject factoren en de verschillende niveaus die daartoe behoren labellen. 
- Herhaaldelijke meting cellen: De aparte kolommen in de data die de niveaus van de binnen-subect factor(en) weergeven. 
- Tussen subject factoren: Deze variabele kan worden geselecteerd als de participanten in twee of meer groepen zijn ingedeeld. 
- Covariaten: In dit veld kan je een covariaat selecteren. Covariaten zijn continue variabelen die een invloed op de afhankelijke variabele hebben maar geen deel zijn van de manipulatie.


### Model
- Componenten en model termen:
  - Herhaaldelijk meten componenten: Alle binnen-subject factoren en covariaten die kunne worden meegenomen in het model. 
	  - model termen: De binnen-subject factoren en covariaten die meegenomen kunnen worden.
  - Tussen-subject componenten: Alle tussen-subject factoren die kunnen worden meegenomen in het model.
	  - model termen: De tussen-subject factoren die kunnen worden meegenomen in het model.

- Kwadratensom: Er zijn verschillende soorten de kwadratensom. De keuze van het type is belangrijk als er meerdere factoren zijn en de data ongebalanceerd is. In een ongebalanceerd design hebben de verschillende niveaus van de onafhankelijke variabele niet evenveel observaties (bijv. als een groep meer observaties heeft dan de ander). In dit scenario kan het type kwadratensom de resultaten beïnvloeden.   
    - Type I: Sequentiele kwadratensom. Het verminderen van fouten wanneer elke factor van het model wordt toegevoegd aan de factoren die al mee worden genomen, hiermee wordt de volgorde van factoren in het model behouden. Het resultaat hangt af van de volgorde waarin de factoren aan het model worden toegevoegd. Wanneer het model meer dan een factor bevat is het belangrijk hierover na te denken. 
    - Type II: Hierarchische/gedeeltelijke sequentiele kwadratensom. Het verminderen van fouten wanneer elke factor wordt toegevoegd aan het model dat alle andere factoren bevat, behalve de factoren waar de factor deel van uitmaakt, bijvoorbeeld interacties die deze factor bevatten. Langsrud (2003) adviseert dit type bij een ANOVA met on gebalanceerde data. 
    - Type III: Gedeeltelijke kwadratensom. Het verminderen van fouten wanneer elke factor wordt toegevoegd aan het model dat alle andere factoren bevat, inclusief interacties met deze factor. Dit type wordt vaak geselecteerd, omdat het rekening houdt met interacties (Langsrud, 2003). Dit is de standaardoptie. 
- Gebruik multivariaat model voor vervolgtoetsen: Als je deze optie selecteert wordt het multivariate lineaire model gebruikt in plaats van het aov model voor vervolgtoetsen (contrasten, post-hoc toetsen, marginale gemiddelden). Multivariate toetsen passen waarschijnlijk een betere correctie toe voor schendingen van de sphericiteit assumptie. 

### Assumptie checks
- Sphericiteit toetsen: Sphericiteit houdt in dat de varianties van de verschillen tussen condities van de herhaalde meting gelijk zijn.
- Sphericiteit correcties: Als er niet aan de assumptie van sphericiteit is voldaan is de kans op een type 1 fout hoger. Correcties voor sphericiteit zijn een manier om dit te verminderen.
  - Geen: Er wordt geen correctie uitgevoerd.
  - Greenhouse-Geisser: Deze correctie varieert tussen 1/(k − 1) en 1, waar k staat voor het aantal herhaalde metingen.
  - Huyn-Feldt: Een andere populaire correctie om de vrijheidsgraden te corrigeren.
  
- Homogeniteits toets: Als je deze optie selecteert zal worden geanalyseerd of de variantie van de afhankelijke variabele gelijk is tussen groepen door middel van Levene's toets. 

### Contrasten
Voor elke binnen-subject factor kan een specifiek contrast worden geselecteerd door op "geen" teklikken in de rechter kolom.
- Factoren: Dit zijn de binnen-subject factoren in de analyse. 
- Contrasten: Contrasten maken geplande vergelijkingen mogelijk. Er zijn verschillende contrasten die verschillende soorten vergelijkingen mogelijk maken.  
- Geen: Als je deze optie selecteert worden er geen contrasten uitgerekend. Dit is de standaardoptie. 
    - Afwijking: Als je dit contrast selecteert wordt het gemiddelde van elk niveau van de onafhankelijke variabele vergeleken met het totale gemiddelde (het gemiddelde wanneer alle niveaus samen worden genomen). 
    - Simpel: Als je dit contrast selecteert wordt het gemiddelde van ieder niveau vergeleken met het gemiddelde van een gespecificeerd niveau, bijvoorbeeld met het gemiddelde van de controlegroep. 
    - Verschil: Dit contrast wordt ook wel "reverse Helmert" genoemd. Als je dit contrast selecteert wordt het gemiddelde van elk niveau vergeleken met het gemiddelde van het vorige niveau.
    - Helmert: Als je dit contrast selecteert wordt het gemiddelde van elk niveau vergeleken met het gemiddelde van het volgende niveau. Dit is het omgekeerde van het "verschil" contrast.
    - Herhaald: Als je dit contrast selecteert wordt het gemiddelde van elk niveau vergeleken met het gemiddelde van het volgende niveau. 
    - Polynoom: Dit contrast test polynome trends in de data. Welk specifieke polynoom wordt gebruikt is afhankelijk van het aantal niveaus van de onafhnakelijke variabele. De graad van de trend die wordt gebruikt is het aantal niveaus min 1. Als de onafhankelijke variabele dus bestaat uit twee niveaus wordt een lineaire trend geanalyseerd. Als de onafhankelijke variabele bestaat uit drie niveaus worden een kwadratische trend en een lineaire trend geanalyseerd. 
      - custom: Hier kunnen de contrast gewichten handmatig ingevoerd worden. Van de gewichten dient minstens 1 gewicht niet gelijk aan 0 te zijn.
   
   

### Post-hoc Toetsen
Sleep een of meer namen van factoren naar de rechter kolom om een post-hoc test uit te voeren. Er zijn verschillende opties: 
- effectgrootte: Als je deze optie selecteert wordt de effectgrootte weergegeven (i.e., de grootte van het geobserveerde effect). De gebruikte maat voor de effectgrootte is Cohen's d. De effectgrootte wordt alleen weergegeven voor het post-hoc type `Standaard`.
- betrouwbaarheidsintervallen: Als je deze optie selecteert wordt er een betrouwbaarheidsinterval voor het gemiddelde verschil berekend. Dit wordt voor welke post-hoc methode gedaan, behalve voor Dunn.  De standaardoptie is een interval van 95%. Dit kan tot het gewenste percentage worden aangepast.
- samengevoegde foutterm voor binnen-subject factoren: een samengevoegde foutterm die aanneemt dat de varianties van de contrast scores ongeveer gelijk zijn (i.e., assumptie van sphericiteit) zie Morey (2008) voor details. 
  - Correctie: Om te corrigeren voor meerdere vergelijkingen en type 1 fouten te voorkomen bestaan er verschillende methoden om de p waarde te corrigeren: 
	  - Tukey: Vergelijk alle mogelijke paren van groepsgemiddelden. Deze correctie kan worden gebruikt wanneer de niveaus op de onafhankelijke variabele een gelijke steekproefgrootte en variantie hebben. Deze methode wordt veel gebruikt en is de standaardoptie.
      - Scheffe: Het aanpassen van significatieniveaus in een lineare regressie om rekening te houden met meerdere vergelijkingen. Deze methode is vrij conservatief.
      - Bonferroni: Deze correctie wordt gezien als vrij conservatief. Het risico op een type 1 fout wordt verminderd, maar statistische kracht (power) wordt ook lager.
      - Holm: Deze methode wordt ook wel sequentiële Bonferroni genoemd, en wordt gezien als minder conservatief dan de Bonferronimethode.

### Beschrijvende grafieken 
- Selecteer de onafhankelijke variabele op de horizontale as om een beschrijvende grafiek te maken. Als er meerdere onafhankelijke variabelen zijn kunnen de variabelen in een plot worden weergegeven door de andere variabele in het veld Aparte lijnen te zetten. De variabelen kunnen ook in aparte grafieken worden weergegeven door de andere variabele in het veld Aparte grafieken te zetten.
  - Factoren: De onafhankelijke variabelen die mee worden genomen in de analyse
  - Horizontale as: Selecteer de onafhankelijke variabele die op de horizontale as wordt weergegeven.
  - Aparte lijnen: Door een onafhankelijke variabele in dit veld te plaatsen corresponderen verschillende lijnen met verschillende niveaus van de geselecteerde variabele.
  - Aparte grafieken: Door een onafhankelijke variabele in deze box te plaatsen corresponderen verschillende grafieken met verschillende niveaus van de geselecteerde variabele.
  - label y-as: Het label van de y-as kan handmatig worden bijgesteld.

- weergeven:
	- Geef foutbalken weer: Als je deze optie selecteert worden er foutbalken weergegeven in de grafiek. De foutbalken kunnen ofwel betrouwbaarheidsintervallen ofwel standaardfouten weergeven. Om precieze intervallen of standaard fouten te krijgen wordt de data genormaliseerd door de gemiddelde van elke observatie af te trekken en dan het totale gemiddelde aan elke observatie op te tellen. De varianties van de resulterende genormaliseerde waardes in elke conditie, en daardoor de grootte van de foutbalken, hangt niet langer af van de participant effecten en zijn zo een meer accurate representatie van de experimentele manipulatie. Zie Morw (2008) voor meer detail. 
		- betrouwbaarheidsinterval: Dit is de standaardoptie. Met deze optie geven de foutbalken betrouwbaarheidsintervallen van het gemiddelde van elke combinatie van onafhankelijke variabelen weer. De standaardoptie is een interval van 95%. Dit kan tot het gewenste percentage worden aangepast.
        - Standaardfout: Als je deze optie selecteert geven de foutbalken de standaardfouten van de gemiddelden van elke combinatie van niveaus van de onafhankelijke variabele weer. 
    - Gemiddelde over ongebruikte binnen-subject factoren: Wanneer er meerdere binnen-subject factoren in het model zijn maar je slecht een subject van de factoren plot wordt het gemiddelde over de ongebruikte binnen-subject factoren genomen. Als er bijvoorbeeld twee binnen-subject factoren zijn met twee niveaus, A(1&2) en B (1&2), en alleen A geselecteerd wordt voor de grafiek wordt het gemiddelde van B over de niveaus genomen. Dit betekent dat wanneer het gemiddelde van A1 wordt geplot, het eigenlijk het gemiddelde is van A1B1 en A1B2. Deze procedure wordt beschreven in Loftus & Masson (1994). Wanneer dit veld niet is aangevinkt worden de gemiddelden niet genomen en worden de kolommen A1B1 en A1B2 simpelweg geconcateneerd.
  
### Additionele Opties:
- Marginale gemiddelden: Als je deze optie selecteert wordt het gemiddelde van elk niveau van de onafhankelijke variabele gecorrigeerd voor alle andere variabelen in het model. 
- Vergelijk marginale gemiddelden met 0: Als je deze optie selecteert worden de gecorrigeerde gemiddelden vergeleken met 0 en worden de betrouwbaarheidsintervallen voor de gecorrigeerde gemiddelden berekend.  
	  - betrouwbaarheidsinterval correctie: De betrouwbaarheidsintervallen kunnen op verschillende manieren worden aangepast.
		- Geen: Als je deze optie selecteert wordt er geen correctie gedaan. 
        - Bonferroni: Bonferroni correctie voor de betrouwbaarheidsintervallen.
        - Sidak: Sidak correctie voor de betrouwbaarheidsintervallen.
- Van `...` bootstraps: Als je deze optie selecteert worden de ge-bootstrapte marginale gemiddelde berekend. De standaard hoeveelheid iteraties is 1000. Je kan dit aanpassen tot het gewenste aantal.
- Weergeven:
	- Beschrijvende statistieken: Als je deze optie wordt geselecteert worden het gemiddelde, de standaardafwijking en de steekproefgrootte weergegeven voor iedere combinatie van niveaus van de onafhankelijke variabele.
    - Schattingen van effectgrootte: Als je deze optie selecteert kan je de specifieke type berekening van de effectgrootte selecteren. 
		- &eta;<sup>2</sup> : Als je deze optie selecteert wordt de eta-kwadraat berekend als schatting van de effectgrootte. Deze methode overschat echter de populatie variantie, wat het moeilijk maakt om het effect van dezelfde variabele te vergelijken tussen verschillende onderzoeken (Goss-Sampson, 2018).       
		- partial &eta;<sup>2</sup> : Als je deze optie selecteert wordt de gedeeltelijke eta-kwadraat berekend als schatting van de effectgrootte. Deze methode lost het probleem van overschatting van populatievariantie op, wat het makkelijker maakt om het effect van dezelfde variabele te vergelijken tussen verschillende onderzoeken (Goss-Sampson, 2018).
        - &omega;<sup>2</sup> : Als je deze optie selecteert wordt de omega-kwadraat uitgerekend als schatting van de effectgrootte. Dit wordt gezien als een goede schatter bij een kleine steekproefgrootte (Goss-Sampson, 2018).
    - Vovk-Selke maximum p-ratio: De grens 1/(-e p log(p)) wordt afgeleid van de vorm van de verdeling van de p-waardes. Onder de nul hypotheses (H<sub>0</sub>)  is het uniform (0,1) en onder de alternatieve hypothese (H<sub>1</sub>) neemt hij af in p, bijv. een beta (α, 1) verdeling waar 0 < α < 1. De Vovk-Selke MPR wordt verkregen door het vorm van α onder de alternatieve hypothese te kiezen zodat de p-waarde maximaal diagnostisch is. De waarde is dat de ratio van de dichtheid op punt p onder H<sub>0</sub> en H<sub>1</sub>. Als de tweezijdige p-waarde bijvoorbeeld .05 is is de Vovk-Sellke MPR 2.46. Dit geeft aan dat deze p-waarde maximaal 2.46 zo waarschijnlijk is onder H1 dan onder H<sub>0</sub>. Meer informatie vind je in deze <a href="https://jasp-stats.org/2017/06/12/mysterious-vs-mpr/">blogpost</a>.

### Simpele hoofdeffecten:
De simpele hoofd effecten geven het effect van een binnen-subject factor voor elk niveau van de andere binnen-subject factor door een ANOVA uit te voeren voor elke subset van de data zoals gespecificeerd door de moderator variabele.
  - Factoren: Dit veld bevat alle binnen subject variabelen die worden meegenomen in de analyse.
  - Simpel effect factor: In dit veld selecteer je de binnen subject variabelen om het effect van deze variabele geconditioneerd op de niveaus van de moderator te berekenen. 
  - Moderator factor 1: In dit veld selecteer je de binnen subject variabele die de verschillende niveaus weergeeft. 
  - Moderator factor 2: in dit veld kan je optioneel een aanvullende binnen-subject variabele toevoegen.

- Samengevoegde error term: Een samengevoegde error term maakt de assumtie dat de varianties van de contrastscores ongeveer gelijk zijn (i.e., assumptie sphericiteit).

#### Non-parametrische toetsen 
De Friedman toets is een non-parametrisch alternatief voor de repeated-measures ANOVA wanneer er een compleet blok design is. The Durbin toets wordt automatisch geselecteerd wanneer er een incompleet blok design is. 
- Factoren: Dit veld bevat alle binnen-subject factoren uit de analyse.
- RM factor: De binnen-subject factor waarin je geïnteresseerd bent.
- Optionele groeperings factor: Het is mogelijk om hier een tussen-persoon factor toe te voegen.
- Conover's post-hoc toetsen: Conover's post-hoc toets voor paar wijze vergelijkingen, wanneer de non-parametrische toets statistiek significant is.


### Uitvoer
---
#### Herhaalde Metingen ANOVA
Binnen-Subject Effecten:
- Spericiteit correctie: De geselecteeerde correcties wanneer de assumptie van sphericiteit is geschonden.
- kwadratensom: De opgetelde kwadraten van binnen groep-gemiddelden verschillen. 
- df: vrijheidsgraden.
- Mean square: Schatting van de populatie variantie (de dum of squares gedeeld door de vrijheidsgraden).
- F: De waarde van de F statistiek.
- P: De p-waarde.

Tussen-Subject Effecten:
- kwadratensom: De opgetelde kwadraten van tussen groep-gemiddelden verschillen. 
- df: Vrijheidsgraden.
- Mean Square: Schatting van de populatievariantie (de sum of squares gedeeld door de vrijheidsgraden).
- F: De waarde van de F statistiek.
- P: De p-waarde.

#### Assumptie Checks
Toets van sphericiteit:
- Mauchly's W: Mauchly's W toets statistiek 
- p: p-waarde.
- Greenhouse-Geisser &epsilon;: De Greenhouse-Geisser correctie. Een waarde van 1 geeft aan dat er aan de assumptie sphericiteit is voldaan en een waarde < 1 geeft aan dat sphericiteits geschonden.
- Huynh-Feldt &epsilon;: De Huynh-Feldt correctie.

#### Contrasten
Afwijking/Simpel/Verschil/Helmert/Repeated/Polynoom/Custom Contrast:
- Vergelijking: De niveaus van de onafhankelijke variabele die worden vergeleken. Voor de custom contrasten wordt het gewicht getoond voor elk niveau van de factor.
- Schatting: Het geschatte gemiddelde verschil tussen de vergeleken niveaus.
- SE: De standaardfout van het geschatte gemiddelde.
- df: De vrijheidsgraden van het model.
- t: De waarde van de t-statistiek.
- p: De p-waarde.


#### Post-Hoc Toetsen
Post-Hoc Vergelijkingen:  
- De eerste twee kolommen geven de niveaus van de onafhankelijke variabele die met elkaar worden vergeleken.
- Gemiddelde verschil: Het gemiddelde verschil tussen de niveaus.
- % BI voor gemiddelde verschil: het betrouwbaarheidsinterval voor het gemiddelde verschil tussen de vergeleken niveaus. 95% is de standaardoptie.
	- Linker: De linkergrens van het betrouwbaarheidsinterval.
    - Rechter: De rechtergrens van het betrouwbaarheidsinterval.

- SE: De standaardfout van het gemiddelde verschil.	
- t: De waarde van de t statistiek.
- Cohen's D: De effectgrootte Cohen's d. Deze corrigeert niet voor meerdere vergelijkingen.
- p<sub>tukey</sub>: Tukey's gecorrigeerde p-waarde voor meerdere vergelijkingen.
- p<sub>scheffe</sub>:  Scheffe's gecorrigeerde p-waarde voor meerdere vergelijkingen.
- p<sub>bonf</sub>: Bonferroni's gecorrigeerde p-waarde voor meerdere vergelijkingen.
- p<sub>holm</sub>: Holm's gecorrigeerde p-waarde voor meerdere vergelijkingen. 

Simpele Hoofdeffecten:
- Niveau: De niveaus op de onafhankelijke variabele die worden vergeleken. 
- kwadratensom: De som van kwadraten tussen niveau-gemiddelden verschil.
- df: De vrijheidsgraden van het model.
- Mean Square: Schatting van de populatievatiantie (de sum of squares gedeeld door de vrijheidsgraden).
- F: De waarde van de F statistiek.
- P: De p-waarde.

#### Marginale gemiddelden
Marginale gemiddelden - onafhankelijke variabele: 
- De eerste kolom bevat de niveaus van de onafhankelijke variabele.
- Marginale gemiddelde: Het marginale gemiddelde voor elk niveau van de onafhankelijke variabele. Dit gemiddelde is gecorrigeerd voor alle andere variabelen in het model. 
- SE: De standaardfout van het marginale gemiddelde.
- Linker BI: De linker grens van het betrouwbaarheidsinterval.
- rechter BI: De rechter grens van het betrouwbaarheidsinterval.
- t: De waarde van de t-statistiek.
- p: De p-waarde.


#### Marginale gemiddelde door middel van Bootstrapping
Ge-bootstrapte marginale gemiddelden - Binnen-subject variabele: 
- Binnen-subject variabele: Deze kolom bevat alle niveaus van de binnen-subject variabele.
- Marginale gemiddelde: Schatting van het marginale gemiddelde voor elk niveau van de binnen-subject variabele. Dit gemiddelde is gecorrigeerd voor alle andere variabelen in het model. De schatting is gebaseerd op de bootstrap verdeling.
- Bias: Bias: Het gemiddelde verschil tussen het ge-bootstrapte marginale gemiddelde en het geschatte marginale gemiddelde.
- SE: De standaardfout van de ge-Bootstrapte marginale gemiddelden. 
- 95% BI voor gemiddelde verschil: De voor bias-gecorrigeerde betrouwbaarheidsinterval voor het gemiddelde verschil tussen de vergeleken niveaus. De standaardoptie is 95%.
  - Linker: De linkergrens van het betrouwbaarheidsinterval.
  - Rechter: De rechtergrens van het betrouwbaarheidsinterval.

#### Non-parametrische toetsen
Friedman Toets / Durbin Toets:
- Factor: The binnen subject factor van de analyse.
- Chi-kwadraat: De chi-squared toets statistiek.
- df: vrijheidsgraden.
- p: De p-waarde.
- Kendall's W: Kendall’s W Test kan worden gezien als normalisatie van de Friedman/Durbin statistiek.
- F: De waarde van de F statistiek.
- df num: Vrijheidsgraden voor het bepalen van p-waardes van de F-statistieken.
- df den: Vrijheidsgraden voor het bepalen van p-waardes van de F-statistieken.
- p<sub>f</sub>: De p-waarde van de F-statistiek.

Conover's post-hoc Vergelijkingen:
- De eerste twee kolommen representeren de niveaus van de binnen-subject factor die worden vergeleken.
  - T-stat: De toets statistiek die de t-verdeling volgt.
- df: Vrijheidsgraden.
- W<sub>i</sub>: Som van de opgetelde rangen van niveau 1.
- W<sub>j</sub>: Som van de opgetelde rangen van niveau 2.
- p: De p-waarde.
- p<sub>bonf</sub>: Bonferroni gecorrigeerde p-waarde voor meerdere vergelijkingen.
- p<sub>holm</sub>: Holm's Gecorrigeerde p-waarde voor meerdere vergelijkingen. 

#### Beschrijvende grafieken 
De onafhankelijke variabele op de x-as en afhankelijke variabele op de y-as. Als er meerdere binnen-subject factoren worden meegenomen in de analyse kunnen deze ofwel met verschillende lijnen, ofwel in verschillende grafieken worden weergegeven.

### Referenties
---
- Conover,W. J. (1999). *Practical nonparametric Statistics, 3rd. Edition*, Wiley.
- Morey, R. D. (2008) Confidence intervallen from Normalized Data: A correction to Cousineau (2005). Tutorial in *Quantatitative Methods for Psychology, 4*(2), 61-64.
- Loftus, G. R., & Masson, M. E. J. (1994). Using confidence intervallen in within-subject designs. *Psychonomic Bulletin and Review, 1*, 476–490.

### R-packages
---
- afex
- boot
- emmeans
- ggplot2
- plyr
- stats
