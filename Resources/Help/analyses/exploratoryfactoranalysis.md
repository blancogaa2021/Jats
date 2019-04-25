# Exploratory Factor Analysis 
--- 
With Exploratory Factor Analysis you can identify the factors underlying your data. The factors try to explain the correlations between the variables. 

### Input 
---
#### Asssignment Box: 
- Included Variables: In this box you can drop the variables to perform the exploratory factor analysis on. 

#### Number of Factors: 
Here you decide on how many factors you would like to apply the rotation. For this you can use several methods.  
- Parallell Analysis: Factors are selected on the basis of parallell analysis. With this method factors are selected when their eigenvalue is bigger than the parallel average random eigenvalue. This method is selected by default. 
- Eigenvalues: Factors are selected when they have a certain eigenvalue. By default factors are selected that have an eigenvalue of 1 or higher. This is called the Kaiser criterion. 
- Manual: Here you can set the amount of factors yourself. By default this is set to 1. 

#### Rotation: 
Here you can set the rotation method to apply to the factors.  
- Orthogonal: This method produces factors that are uncorrelated. For this method there are several possibilities you can select. 
    - None: No rotation method is selected. 
    - varimax: Orthogonal rotation method varimax. This rotation based on the maximizing the variance of the loadings. 
    - quartimax: Orthogonal rotation method quartimax. In this rotation method the number of factors that is  necessary to explain each variable is minimized. 
    - bentlerT: Orthogonal rotation method bentlerT. 
    - equamax: Orthogonal rotation method equamax. This is a combination of varimax and quartimax. 
- Oblique: This method produces factors that allow for correlation. This method is selected by default. For this method there are several possibilities you can select. 
    - promax: Oblique rotation method promax. This method is selected by default. 
    - oblimin: Oblique rotation method oblimin. 
    - simplimax: Oblique rotation method simplimax. 
    - bentlerQ: Oblique rotation method bentlerQ. 
    - biquartimin: Oblique rotation method biquartimin. 
    - cluster: Oblique rotation method cluster. 

#### Output Options: 
- Highlight: This option allows you to adjust the thickness of the arrows in the path diagram. By default arrows become thicker when their value is 0.4 or higher. 
- Include Tables: 
    - Factor correlations: When selecting this option, a table with the correlations between the factors will be provided. 
    - Additional fit indices: This option provides you the RMSEA with 90% confidence interval, the TLI, and the BIC to test the fit of the model. 
    - Path diagram: By selecting this option, a visual representation of the direction and strength of the relation between the variable and factor will be provided. 
    - Scree plot: When selecting this option, the output will provide a scree plot. The scree plot provides information on how much variance, indicated by the eigenvalue, in the data is explained by each factor. A scree plot can be used to decide how many factors should be selected. We would like to explain the most variance with the least amount of factors. When by adding a factor not much more variance is explained, this point indicates how many factors should be used in the model. 
- Missing values: 
    - Exclude cases pairwise: If one observation from a variable is missing, all the other variable observations from the same case, will still be used for the analysis. In this scenario it is not necessary to have a observation for all the variables to include the case in the analysis. This option is selected by default. 
    - Exclude cases listwise: If one observation from a variable is missing, the whole case, so all the other connected variable observations, will be dismissed from the analysis. In this scenario observations for every variable are needed to include the case in the analysis. 

### Output 
--- 
#### Factor Loadings: 
- Variables: The first column shows all the variables included in the analysis. 
- PC (1, 2, 3, etc.): This column shows the factorloadings on the variable. 
- Uniqueness: The percentage of the variable variance that is not explained by the factor. 

#### Factor Correlations: 
- The correlation between the factors. The diagonal is always 1 because that is the correlation with the own factor.   

#### Chi-squared Test: 
The fit of the model is tested. When the test is significant, the model is rejected. Bear in mind that a chi-squared approximation is unreliable for small sample sizes. If you have a small sample size you can get additional information about the fit of the model by selecting the option `Additional fit indices` in the `Output options`. 
- Model: The model obtained from the exploratory factor analysis. 
- Value: The test statistic chi-squared.  
- df: Degrees of freedom. 
- p: P-value. 

#### Additional Fit Indices: 
The fit indices provide information about the fit of the model. 
- Model: The model obtained form the exploratory factor analysis. 
- RMSEA: Root Mean Square Error of Approximation. Corrects for parsimony. When models peform the same, but model 1 has more df's than model 2, model 1 will be recommended. Browne and Cudeck (1993) advice a value less than 0.08 for an acceptable model fit, less than 0.05 a good model fit, and advice to reject models with values of 0.1 or higher. However, there is absolute agreement on these cutoffs. 
- RMSEA 90% confidence interval: The 90% confidence interval of the Root Mean Square Error of Approximation 
- TLI: Tucker-Lewis Index. Evaluates the fit compared to a more resticted, nested baseline model. Hopwood and Donnallan (2010) suggested that a value higher than .9 indicates a good fit. However, there is no consensus about this cutoff. 
- BIC: Bayesian Information Criterion. This measure you is useful to compare the performance of different models on the same data, whereas a lower value indicates a better fitting model. 

#### Path Diagram: 
- F(1,2,3,...): The factors in the model are represented by the circles.  
- Variables: The variables are represented by the boxes. 
- Arrows: Going from the factors to the variables, representing the loading from the factor on the variable. Red indicates a negative loading, green a positive loading. The thicker the arrows, the higher the loading. You can adjust this highlight at `Highlight` in the `Output Options`. 

#### Screeplot: 
The scree plot provides information on how much variance, indicated by the eigenvalue, in the data is explained by each factor. The scree plot can be used to decide how many factors should be selected in the model. We would like to explain the most variance with the least amount of factors. The point where adding a factor does not lead to more explained variance, indicates how many factors should be used in the model.
- Factors: On the x-axis, all possible factors. 
- Eigenvalue: On the y-axis, the eigenvalue indicating the variance explained by each factor. 
- Data: The dotted line represents the data. 
- Simulated: The triangle line represents the simulated data. This line is indicative for the parallel analysis. When the points from the dotted line (real data) are above this line, these factors will be included in the model by parallel analysis. 
- Kaiser criterion: The horizontal line at the eigenvalue of 1 represents the Kaiser criterion. According to this criterion, only factors with values above this line (at an eigenvalue of 1) should be included in the model. 

### References 
Brown, T. A. (2014). Confirmatory factor analysis for applied research.     
    Guilford Publications. 
Hayton, J. C., Allen, D. G., & Scarpello, V. (2004). Factor retention     
    decisions in exploratory factor analysis: A tutorial on parallel analysis. Organizational research methods, 7(2), 191-205.
Hopwood, C. J., & Donnellan, M. B. (2010). How should the internal structure 
    of personality inventories be evaluated? Personality and Social Psychology Review, 14, 332–346. 
Osborne, J. W., Costello, A. B., & Kellow, J. T. (2008). Best practices in 
    exploratory factor analysis. Best practices in quantitative methods, 86-99.

### Example 
For an example you can go to `File`-->`Data library`-->`Factor`. 
