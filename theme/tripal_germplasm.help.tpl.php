<h3>Germplasm Management Quick Links</h3>
<li><?php l('Configuration', 'admin/tripal/germplasm/configuration'); ?></li>
<li><?php l('Add Parents to Crossing Block', 'node/add/crossingblock_parents'); ?></li>
<li><?php l('Register Crosses', 'node/add/crosses'); ?></li>
<li><?php l('All Germplasm View', 'germplasm'); ?></li>
<li><?php l('All Crossing Block Summary Page', 'Crossing_Blocks'); ?></li>
<li><?php l('Generate Pedigree', 'germplasm/generate_pedigree'); ?></li>
<br>

<h3>Module Description</h3>
<p>The germplasm module aims to provide a user friendly interface for management of a
breeding program and all resulting germplasm. This includes providing task oriented forms
for assigning parents to crossing blocks & register parents of a crossing block. Furthermore,
a number of default views/tables have been created which provide details specific to given
situations. For example, there are views/tables for displaying progeny or parents of a
specified crossing block.</p>
<br />

<h3>Setup Instructions</h3>
<ol>
<li><b>Install Germplasm Theme:</b>Move the germplasm_theme folder from
sites/all/modules/germplasm_manage to your tripal theme directory. Make sure to clear the
theme cache by navigating to Administer -> Site Building -> Themes</li>
<li><b>Install dynamic_views_columns Views Style Plugin</b>: This can be downloaded from
the Tripal site within the Extensions section and requires Views 2.x.</li>
<li><b>Set Stock Controlled Vocabularies:</b> Go to Administer -> Tripal Manangement ->
Stocks -> Configuration and set the controlled vocabularies to be used for stock property types and
stock relationship types. The stock property cv must contain the following terms: synonym,
crossingblock_season, crossingblock_year. The stock relationship cv must contain the following
terms: is_maternal_parent_of and is_paternal_parent_of.</li>
<li><b>Set Database to contain all Germplasm:</b> By going to Configure above, you can
change the database and accession prefix for the database reference created whenever the various
create germplasm forms are used to create a stock. This ensures that all germplasm are grouped together
with database references for a common database. It's recommended that you create a database
specifically for this field.</li>
<li><b>Set Databases to be used for Cross Numbers (optional)</b>: By going to Configure above,
you can select a number of databases that can be used from cross numbers. These databases should be created
especially for this purpose. All of the databases selected will be presented to the user in the register
crosses form as a choice for the cross database. Cross numbers should be unique to a given database
and are often used in breeding programs to keep track of crosses and individuals.</li>
</ol>

<br>
<p><b>NOTE:</b> All of the stocks/germplasm to be used as parents for crosses must be created
before they can be assigned as a parent. Thus the typical workflow for managing a breeding program
would be to register the parents of a crossing block (the form allows you to create new germplasm if
the parent doesn\t already exist), make the crosses in the field, give all the surviving crosses
cross numbers and then register them with your tripal site using the register crosses form.</p>

<br>
<h3>Features of this Module</h3>
<ul>
<li><b><?php l('Create Generic Germplam', 'node/add/germplasm'); ?></b>: Essentially creates a stock with a main database reference
indicating that it is germplasm.</li>
<li><b><?php l('Add Parents to a Crossing Block','node/add/crossingblock_parents'); ?></b>: Essentially this form allows you to add a
synonym of a controlled form ([season][year]-[parent number]) to an already existing stock or
a newly created stock in order to mark it as a parent of the specified crossing block.</li>
<li><b><?php l('Register Crosses','node/add/crosses'); ?></b>: A simplified form allowing you to add up to 10 crosses/stocks at a
time with each stock/germplasm having 2 relationships (maternal/paternal parent), a main
a database reference (one given to all germplasm), two properties (crossingblock season and year)
and optionally, an additional database reference (cross number); ?></li>
<li><b><?php l('Generate Pedigree','germplasm/generate_pedigree'); ?></b>: A form allowing users to generate
a pedigree diagram and textual short form for any germplasm currently enterered. Either the Parental (Displays all
relationships where the entered stock/germplasm is the object ie: the child in the is_maternal/paternal_parent_of
relationship) or Offspring (Displays all relationships where the entered stock/germplasm is the subject ie: the
parent in the is_maternal/paternal_parent_of relationship) Pedigree.</li>
<li><b>Pedigree on Stock/Germplasm Page</b>: Added a generate pedigree block to all stock/germplasm
details pages that displays a default pedigree and allows the user to change settings just as in the
Pedigree Generator but without having to enter the stock to generate the pedigree for.</li>
<li><b>Views Integration</b>: Drupal Views is a powerful tool that allows the site administrator
to create lists or basic searching forms of Chado content. It provides a graphical interface within Drupal
to allow the site admin to directly query the Chado database and create custom lists without PHP programming
or customization of Tripal source code. Views can also be created to filter content that has not yet been
synced with Druapl in order to protect access to non published data (only works if Chado was installed using
Tripal); ?> You can see a list of available pre-existing Views <?php l('here','admin/build/views'); ?>, as well as
create your own.</li>
<li><b><?php l('All Germplasm', 'germplasm'); ?></b>: a view (sortable, filterable table) listing all germplasm for a
given organism. The default has no organisms selected so that no query is run until the user selects
an organism and clicks filter. This greatly speeds up the initial time to load the view/table
since it doesn\t load a number of records the user doesn\t care about anyway.</li>
<li><b><?php l('Summary of All Crossing Blocks', 'Crossing_Blocks'); ?></b>: a dynamic page that lists all of the crossing
blocks in a table by organism with the number of parents and progeny and a link to a summary listed for each. Each number
links to one of the views described next depending on whether its a number of progeny or parents
and fully describes each member. This summary can be found at crossing blocks (in the same menu as "My Account" and "Create Content"</li>
<li><b>Summary of a single Crossing Block</b>:
Provides a simple table indicating the number of parents currently added to that crossing block as well as
the number of registered crosses/progeny. It also links to the following two views and the forms to add
more parents/progeny/crosses. Essentially this is meant as a launchpad for all data and for adding data for a
given crossing block.
This can be found at Crossing_Block/[Organism Common Name]/[Crossing Block Year]/[Crossing Block Season]/Summary
(ie: Crossing_Block/Common-Bean/2011/Summer/Summary) or reached from the Link in the summary column for a given
crossing block in the "All Crossing Blocks Summary" page.</li>
<li><b>Parents of a given Crossing Block</b>: This is a view displaying all the parents for
a given crossing block including additional details about each one and a link to their germplasm/stock node/details page.
This can be found at Crossing_Block/[Organism Common Name]/[Crossing Block Year]/[Crossing Block Season]/Parents
(ie: Crossing_Block/Common-Bean/2011/Summer/Parents) or reached by clicking on the number of parents
for a given crossing block in the "All Crossing Blocks Summary" page.</li>
<li><b>Progeny/Crosses of a given Crossing Block</b>: This is a view displaying all the progeny for
a given crossing block  inclduing each ones parents and properties and a link to their node/details page.
This can be found at Crossing_Block/[Organism Common Name]/[Crossing Block Year]/[Crossing Block Season]/Progeny
(ie: Crossing_Block/Common-Bean/2011/Summer/Progeny) or reached by clicking on the number of progeny
for a given crossing block in the "All Crossing Blocks Summary" page.</li>
</ul>