Installation
============

.. note::

  It is recommended to clear cashes regularly in this installation processes.


Download Package
------------------

The package is available as one repository for `Pulse Bioinformatics, University of Saskatchewan <https://github.com/UofS-Pulse-Binfo>`_ on GitHub. Recommended method of downloading and installation is using git:

.. code:: bash

  cd [your drupal root]/sites/all/modules

  git clone https://github.com/UofS-Pulse-Binfo/kp_germplasm.git

Enable Package
----------------

The module can be enabled in "Home » Administration » Tripal » Modules" by select the checkbox under "ENABLED" column (as shown in above image) and then click on "Save Configuration" button by the bottom of page.

.. image:: install.1.tripal_module_page.png

Another method that can enable our module is using drush:

.. code:: bash

  drush pm-enable kp_germplasm
  drush pm-enable rilsummary germpcollection


.. note::

  In this step, module required ontologies and controlled vocabularies will be inserted into Chado. **Make sure to run any Tripal jobs created by these modules before continuing.**


Set Permissions
---------------

By default, permission of using both importers in this module is not set. It can be configured in "Home » Administration » People » Permissions".

.. image:: install.4.permission.png

Import Data
------------
After the module is installed and enabled, both Germplasm Cross Importer and Germplasm Accession Importer should be ready to use in "Home » Administration » Tripal » Data Loader".

.. image:: install.2.cross_importer.png

.. image:: install.3.accession_importer.png

For more information on the importers, See the Data Import section of these docs.

.. note::

  The importers add data to from your file into Chado. You then need to publish that data by going to Admin > Content > Tripal Content > Publish and selecting either "F1" for crosses or "Germplasm Accessions".

Upgrade path from Separate Modules
---------------------------------------

This package includes modules which used to stand alone (germ_summary, tripal_germplasm_importer, germcollection). To upgrade,

1. Take note of any existing configuration both in Tripal > Extensions and Structure > Tripal Content Types.
2. Disable and uninstall the existing modules. This will not delete any data in chado; however, you will need to re-configure the functionality.
3. Remove the old module directories.
4. Clone this package and re-install the modules
5. Re-apply the configuration you took note of above.

.. warning::

  You may need to re-configure after upgrading to this package so take careful note of your original configuration.

.. note::

	The functionality from separate modules will still be available in this package and any new functionality will be developed here. Additionally, germ_summary has been renamed to rilsummary to reflect it's focus on RILs.
