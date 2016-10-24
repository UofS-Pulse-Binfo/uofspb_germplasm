# RIL Summary
This module provides a table/matrix per genus where the cells provide a count of the number of chado stocks of type "Recombinant Imbred Line" with a maternal parent of species X and a paternal parent of species Y where X is specified by the column and Y is specified by the row. This results in the number of intraspecific crosses along the center diagonal and interspecific crosses flanking it. If the user clicks on a given count they will be taken to a paged listing of stocks with that parent species combination and if they hover over a count they will be given the first three names.

This module also provides a means for keeping track of RIL development through use of stock properties. The generations from F1 to F8 are shown as columns and map to stock properties "F1", "F2", "F3", etc. If the property is present but with no value then a checkmark appears; if a value is present then it is shown in the table. The final column can be filled in with the "RIL_Complete" stock property.

### Screenshot of the Germplasm Summary as shown on KnowPulse for Lentil:
![Germplasm Summary Screenshot](https://cloud.githubusercontent.com/assets/1566301/18369835/5c703484-75e5-11e6-9392-54fc468f7d33.png)

### Scheenshot of the germplasm listing:
![Germplasm Listing Screenshot](https://cloud.githubusercontent.com/assets/1566301/19665397/0e0de2e6-9a02-11e6-9b35-1a98d27d9a8f.png)
NOTE: The Accession, Maternal/Paternal parent has been obfuscated here for privacy reasons. The parents would be links and the accession is the unique name.
NOTE: The checkmarks appear if the property is present but there is no value (LR-03). If there is a value then it is displayed (LR-05).

## Assumptions
1. Only chado stocks with both parents will be counted. It is assumed that parents are specified using the stock_relationship table with a type of "is_maternal_parent_of" or "is_paternal_parent_of" respectively. The parent should be the subject and the child should be the object of the relationship.
2. If the genus is not provided in the path (ie: [mytripalsite.com]/germplasm/summary/myGenus) then the module will choose the genus with the most species. If there are two that meet that criteria then the one that comes first alphabetically will be chosen.

## Installation
This module requires Tripal Core and is compatible with version 7.x-2.x. It should be installed as any other Drupal module is (place in the correct directory and enable through the interface) and has no additional configuration. __After installation go to [mytripalsite.com]/germplasm/summary to see the germplasm summary for your data.__
