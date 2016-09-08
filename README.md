# Germplasm Summary
This module provides a table/matrix per genus where the cells provide a count of the number of chado stocks with a maternal parent of species X and a paternal parent of species Y where X is specified by the column and Y is specified by the row. This results in the number of intraspecific crosses along the center diagonal and interspecific crosses flanking it. If the user clicks on a given count they will be taken to a paged listing of stocks with that parent species combination and if they hover over a count they will be given the first three names.

### Screenshot of the Germplasm Summary as shown on KnowPulse for Lentil:
![Germplasm Summary Screenshot](https://cloud.githubusercontent.com/assets/1566301/18369835/5c703484-75e5-11e6-9392-54fc468f7d33.png)

### Scheenshot of the germplasm listing:
![Germplasm Listing Screenshot](https://cloud.githubusercontent.com/assets/1566301/18369867/a7c7f368-75e5-11e6-84d8-4526009e2930.png)

## Assumptions
1. Only chado stocks with both parents will be counted. It is assumed that parents are specified using the stock_relationship table with a type of "is_maternal_parent_of" or "is_paternal_parent_of" respectively. The parent should be the subject and the child should be the object of the relationship.
2. If the genus is not provided in the path (ie: [mytripalsite.com]/germplasm/summary/myGenus) then the module will choose the genus with the most species. If there are two that meet that criteria then the one that comes first alphabetically will be chosen.

## Installation
This module requires Tripal Core and is compatible with version 7.x-2.x. It should be installed as any other Drupal module is (place in the correct directory and enable through the interface) and has no additional configuration. __After installation go to [mytripalsite.com]/germplasm/summary to see the germplasm summary for your data.__
