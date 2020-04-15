![Tripal Dependency](https://img.shields.io/badge/tripal-%3E=3.0-brightgreen)
![Module is Generic](https://img.shields.io/badge/generic-tested%20manually-yellow)

# KP Germplasm

Provides specialized Tripal fields and importers for germplasm. This package is a collection of modules for to ensure you have choice over what functionality most suits your datasets and Tripal site purpose.

## Module Summary

 - KP Germplasm: provides data importers and general fields for all germplasm.
 - RIL Summary: provides a germplasm matrix summarizing RIL parentage including species.
 - Germplasm Collection: provides specialized fields for supporting germplasm collections.

## Upgrade path from Separate Modules
This package includes modules which used to stand alone (germ_summary, tripal_germplasm_importer, germcollection). To upgrade,

1. Take note of any existing configuration both in Tripal > Extensions and Structure > Tripal Content Types.
2. Disable and uninstall the existing modules. This will not delete any data in chado; however, you will need to re-configure the functionality.
3. Remove the old module directories.
4. Clone this package and re-install the modules
5. Re-apply the configuration you took note of above.
