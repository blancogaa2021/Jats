Bayesian Linear Regression
==========================

The Bayesian Linear Regression allows you to model a linear relationship between one or more explanatory variable(s) and a continuous dependent variable. This analysis uses the BAS package which implements Bayesian Model Averaging and Model Selection using state of the art prior distributions for linear and generalized linear models.

#### Assumptions
- Continuous dependent variable

Options
-------

#### Bayes Factor:
- *BF10*: Bayes factor to quantify evidence for the alternative hypothesis relative to the null hypothesis
- *BF01*: Bayes factor to quantify evidence for the null hypothesis relative to the alternative hypothesis
- *Log(BF10)*: Natural logarithm of *BF10*

#### Order:
Compares each model against the model selected.
- Compare to best model
- Compare to null model

#### Number of models shown
By default, the output shows all the models computed. There is an option to show the best *n* models.

#### Output
- *Posterior summary*: Output table containing the Marginal Posterior Summaries of Coefficients. Options available to compare against - Best model, Median model, Most complex model, Model averaged
- *Descriptives*: Output table contains Descriptive statistics (mean, standard deviation and size) about the variables selected.

Model
-----
Allows Model terms (Covariates) to be included in the Null model.

Plots
-----

#### Coefficients:
- *Inclusion probabilities*: Shows a histogram of the posterior inclusion probabilities. The dotted line displays the prior inclusion probabilities.
- *Averaged posterior distributions*: Shows t-approximations of the posterior distributions averaged over all models.

#### Models:
- *Posterior log odds*: Shows a heatmap of the log posterior odds against the model rank.
- *Log(P(data | M)) vs. model size*: Shows the relation between model fit and complexity.
- *Model probabilities*: Displays the cumulative distribution function of the model search.

#### Residuals
- *Residuals vs. fitted*: Plots the residuals of the model averaged predictions (BMA) against the residuals.

Advanced Options
----------------

#### Prior:
Prior distribution for regression coefficients. Choices include

- *AIC*: Compare models using the Akaike Information Criterion.
- *BIC*: Compare models using the Bayesian Information Criterion.
- *EB-global*: Global Empirical Bayes estimates of g in Zellner-Siow g-prior and model probabilities. Uses an EM algorithm to find a common or global estimate of g, averaged over all models. When it is not possible to enumerate all models, the EM algorithm uses only the models sampled under EB-local.
- *EB-local*: Uses the MLE of g from the marginal likelihood within each model.
- *g-prior*: Zellner's g-prior
- *Hyper-g*: A mixture of g-priors where the prior on g/(1+g) is a Beta(1, alpha/2) as in Liang et al (2008). This uses the Cephes library for evaluation of the marginal likelihoods and may be numerically unstable for large n or R2 close to 1. Default choice of alpha is 3
- *Hyper-g-Laplace*:  Same as *Hyper-g* but uses a Laplace approximation to integrate over the prior on g.
- *Hyper-g-n*: A mixture of g-priors that where u = g/n and u  Beta(1, alpha/2) to provide consistency when the null model is true.
- *JZS*: " Jeffreys-Zellner-Siow prior which uses the Jeffreys prior on sigma and the Zellner-Siow Cauchy prior on the coefficients. The optional parameter can be used to control the squared scale of the prior (default r-scale: 0.354).

#### Model prior:
Prior distribution on the models.
- *Beta binomial*: Default Beta(a = 1, b = 1)
- *Beronoulli*: Default p = 0.5
- *Uniform*

#### Sampling method:
Indicates the sampling method to be used
- *BAS*: Uses Bayesian Adaptive Sampling (without replacement). These can be updated based on estimates of the marginal inclusion. *No. models* indicates the number of models to sample without replacement. Setting the value to 0 implies the analysis will attempt to enumerate all models.
- *MCMC*: Samples with replacement via a MCMC algorithm that combines the birth/death random walk with a random swap move to interchange a variable in the model. *No. samples* indicates the number of MCMC samples to draw. Setting the value to 0 implies the number of MCMC iterations is equal to 10 times the number of models. Sampling stops when min(number of models, MCMC iterations) is reached. 

It is recommended to use *BAS* when the model space can be enumerated.

Output Tables
-------

#### Model Comparison:
- *Models*: Predictors contained in the model.
- *P(M)*: Prior model probabilities.
- *P(M | data)*: Posterior probabilites of the models considered.
- *BFM*: Posterior model odds.
- *BF10* (or *BF01*): Bayes factor.
- *R2*: Explained variance.

#### Marginal Posterior Summaries
Marginal Posterior summaries of Coefficients.
- *Coefficient*: Name of the predictors.
- *Mean*: Mean of the model averaged posterior.
- *SD*: Standard Deviation of the model averaged posterior.
- *P(incl | data)*: Posterior inclusion probability.


References
-------
- Clyde, M. A. (2017). BAS: Bayesian Adaptive Sampling for Bayesian Model Averaging. (Version 1.4.7)[Computer software].
- Clyde, M. A., Ghosh, J., & Littman, M. L. (2011). Bayesian adaptive sampling for variable selection and model averaging. Journal of Computational and Graphical Statistics, 20, 80-101.
