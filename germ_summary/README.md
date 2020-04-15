![Tripal Dependency](https://img.shields.io/badge/tripal-%3E=3.0-brightgreen)
![Module is Generic](https://img.shields.io/badge/generic-tested%20manually-yellow)
![GitHub release (latest by date including pre-releases)](https://img.shields.io/github/v/release/UofS-Pulse-Binfo/germ_summary?include_prereleases)

# RIL Summary
Provides functionality for summarizing Recombinant Inbred Lines (RILs) including the following:
1. Tabular matrix which summarizes how many RILs are available for each species combination. This is particularily helpful if you have a cultivated and associated wild species for a single genus.
2. Listing of all RILs for a specific species combination including information about the number of F2 families for each F-generation.
3. ChadoField for RIL pages which summarizes information about the number of F2 families for each F-generation.

### Screenshots
![Germplasm Summary Screenshot](https://user-images.githubusercontent.com/1566301/65840297-8c854880-e2d4-11e9-9500-4edb8d94e61f.png)
![Germplasm Listing Screenshot](https://user-images.githubusercontent.com/1566301/65840304-9e66eb80-e2d4-11e9-834c-02f94f842c4a.png)
![RIL Summary field](https://user-images.githubusercontent.com/1566301/65840310-b8083300-e2d4-11e9-97ba-8e02850dd8f3.png)

## Installation
This module requires Tripal Core and is compatible with version 7.x-3.x. It should be installed as any other Drupal module is (place in the correct directory and enable through the interface) and has no additional configuration. __After installation go to [mytripalsite.com]/germplasm/summary to see the germplasm summary for your data.__

## Adding RILs to the summary.
1. Create a `Recombinant Inbred Line` with the name of your RIL population (e.g. TR-01).
2. Create a germplasm line (type doesn't matter; suggested `Generated Germplasm (Breeding Line)`) with the name of the original cross giving rise to the RIL population (e.g. 1234S) and add a relationship: `TR-01 is_selection_of 1234S`.
2. Create parents for the Breeding cross (type does not matter) and related them using the is_maternal_parent and is_paternal_parent relationship types (e.g. `CDC FRED is_maternal_parent_of 1234S` and `AABC is_paternal_parent_of 1234S`).

## Adding the summary to RIL pages.
1. Go to Admin > Structure > Tripal Content Types > Recombinant Inbred Lines > Manage Fields.
2. Add a new field where the type is `Germplasm RIL Summary`.
3. Make sure it is not disabled on the `Manage Display` tab.
