![Tripal Dependency](https://img.shields.io/badge/tripal-%3E=3.0-brightgreen)
![Module is Generic](https://img.shields.io/badge/generic-tested%20manually-yellow)

# UofS Pulse Binfo Germplasm

Provides specialized Tripal fields and importers for germplasm. This package is a collection of modules for to ensure you have choice over what functionality most suits your datasets and Tripal site purpose.

## Module Summary

 - UofS Pulse Binfo Germplasm: provides data importers and general fields for all germplasm.
 - RIL Summary: provides a germplasm matrix summarizing RIL parentage including species.
 - Germplasm Collection: provides specialized fields for supporting germplasm collections.

## Installation

See [our documentation for detail installation and configuration instructions.](https://kp-germplasm.readthedocs.io/en/latest/install/install.html)

If you were using germ_summary, tripal_germplasm_importer, germcollection then you will need to upgrade to this package to access new functionality and bug fixes. To upgrade, see [our documentation for detailed instructions.](https://kp-germplasm.readthedocs.io/en/latest/install/install.html#upgrade-path-from-separate-modules)

**Note: the functionality from separate modules will still be available in this package and any new functionality will be developed here. Additionally, germ_summary has been renamed to rilsummary to reflect it's focus on RILs.**

## Documentation [![Documentation Status](https://readthedocs.org/projects/kp-germplasm/badge/?version=latest)](https://kp-germplasm.readthedocs.io/en/latest/?badge=latest)

There is extensive documentation available for this package through ReadtheDocs. You can [access our documentation here.](https://kp-germplasm.readthedocs.io/en/latest/index.html)

## Automated Testing ![Run PHPUnit Tests](https://github.com/UofS-Pulse-Binfo/uofspb_germplasm/workflows/Run%20PHPUnit%20Tests/badge.svg)

Automated testing for this module was implemented using [Tripal Test Suite](https://github.com/tripal/TripalTestSuite) and is run automatically using Travis CI. You can see the build status above.

## Funding

This work is supported by Saskatchewan Pulse Growers [grant: BRE1516, BRE0601], Western Grains Research Foundation, Genome Canada [grant: 8302, 16302], Government of Saskatchewan [grant: 20150331], and the University of Saskatchewan.

## Citation

Shen Y, Sanderson LA, Tan R (2019) KP Germplasm: providing extended Tripal support of germplasm data for breeding program and genebanks. Development Version. University of Saskatchewan, Pulse Crop Research Group, Saskatoon, SK, Canada.
