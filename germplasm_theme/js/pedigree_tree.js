
pedigree_tree = function (canvas, rawtree) {
  this.rawtree = rawtree;
  this.canvas = canvas;
  
  this.nodes = new Array();
  this.connections = new Array();

  this.error = this.canvas.text(10,350,"");
  this.error.attr('text-anchor','start');
  
  //-----------------------------------------------------------------------------------------------
  // DEFAULTS
  
  // then number of pixels between the edge of the text and the rectangle on each side
  this.node_padding = 4;
  
  // the colour attribute of the rectangle (node)
  // either a named colour, a hex code or a gradient (degrees-firstcolor-secondcolor)
  this.rect_color = '90-silver-white';
  
  // the color of the rectangle outline
  this.rect_stroke_color = 'silver';
  
  // the color of the lines connecting each node
  this.connection_stroke_color = 'grey';
  
  // the color surrounding the relationship label
  this.rel_stroke_color = 'lightgrey';
  
  // the fill of the relationship label
  this.rel_fill_color = 'white';

  // key defaults ---
  // the fill of the key header
  this.key_header_color = '90-silver-white';
  this.key_color = 'white';
  this.key_header_txt_color = 'black';
  this.key_outline_stroke = 'silver';
  
  // default level height
  this.level_height = 100;

  this.relationship_colors = new Array();
  this.relationships = new Array();
  
  // draw info box
  this.info_box_fill = '90-#B7C3D0-white';
  this.info_box_icon = '#104E8B';
  this.info_box_stroke = 'silver';
  this.info_box_text = 'black';
    var info_box_icon = this.info_box_icon;
    var info_box_fill = this.info_box_fill;
    var info_box_stroke = this.info_box_stroke;
    var info_box = this.info_box;
    
  //-----------------------------------------------------------------------------------------------  
  this.get_canvas = function () {
    return this.canvas;
  };

  //-----------------------------------------------------------------------------------------------
  this.get_rawtree = function () {
    return this.rawtree;
  };
  
  //-----------------------------------------------------------------------------------------------
  // Draw an information box to be hidden until needed
  this.draw_info_box = function () {
    this.info_box = this.canvas.set(
      this.canvas.rect(this.canvas.width - 205, 5, 200, this.level_height/2, 5).attr({'fill':this.info_box_fill, 'stroke':this.info_box_stroke}),
      this.canvas.set(
        this.canvas.circle(this.canvas.width - 205, 22, 10).attr({'fill':'white','stroke':this.info_box_stroke}),
        this.canvas.circle(this.canvas.width - 205, 22, 8).attr({'fill':this.info_box_icon,'stroke':this.info_box_stroke}),
        this.canvas.text(this.canvas.width - 205, 22, 'i').attr({'fill':'white'})
      ),
      this.canvas.text(this.canvas.width - 185, this.level_height/4+5, "").attr({'fill':this.info_box_text,'text-anchor':'start'})
    ); 
    //this.info_box.hide();
  };
  
  // Draw information box when pedigree_tree created
  this.draw_info_box();  

  //-----------------------------------------------------------------------------------------------  
  // keys: an array of keys specifying a node in rawtree
  //    (ie: if keys = [0, 'children', 2] the element at rawtree[0]['children'][2] would be returned
  this.get_rawnode = function (keys) {
    
    var raw_node = this.rawtree;
    
    var current_element = 'rawtree';
    for (k in keys) {
      current_element = current_element + '['+ keys[k] +']';
      if (typeof(raw_node[keys[k]]) == 'undefined') {
        throw new Error('Element '+current_element+' doesn\'t exist');
      } else {
        raw_node = raw_node[keys[k]];
      }
    }
    
    return raw_node;
    
  };
  
  //-----------------------------------------------------------------------------------------------
  // generate the pedigree including nodes and connections
  this.generate_tree = function() {
    this.draw_info_box;
    
    this.generate_all_nodes();
    //console.warn('Colours (before):'+dump(this.relationship_colors));
    //console.warn('Tree:'+dump(this.rawtree));
    
    this.generate_all_connections([],this.rawtree);
    //console.warn('Colours (after):'+dump(this.relationship_colors));
  };
  
  //-----------------------------------------------------------------------------------------------
  // generate all nodes for the tree
  this.generate_all_nodes = function(options, lvlarray) {
    if (typeof(lvlarray) == 'undefined') {
      lvlarray = this.rawtree;
    }
    
    if (typeof(options) == 'undefined') {
      options = new Array();
    }
    if ( typeof(options['void_length']) == 'undefined' ) {
      options['level_width'] = this.canvas.width;
      options['width_offset'] = 0;
      options['level_height'] = this.level_height;
      options['height_per_level'] = this.level_height;
      options['num_voids'] = lvlarray.length * 2;
      options['void_length'] = options['level_width'] / options['num_voids'];
    }
    
    //generate a node and nodes for all it's children recursively for each base element in lvlarray
    //alert("Level: " + dump(lvlarray));
    var cur_void = 0;
    for (n in lvlarray) {
      var name = lvlarray[n]['name'];
      if (n == 0) {
        if ( lvlarray[n]['shared'] == 'TRUE') {
          // move one void closer to other parent
          cur_void = cur_void + 2;
        } else {
          // dealing with in between two nodes thus two voids (1 flanking each node)
          cur_void = cur_void + 1;
        }
      } else {
        if ( lvlarray[n]['shared'] == 'TRUE') {
          // move one void closer to other parent
          cur_void = cur_void + 3;
        } else {
          // dealing with in between two nodes thus two voids (1 flanking each node)
          cur_void = cur_void + 2;
        }
      }
      options['cur_void'] = cur_void;
  
      // generate current node..........
      this.generate_node(lvlarray[n], options);
      

      
      //generate nodes for all children of current node............
      newopt = options.slice(0);
      newopt['level_width'] = options['level_width'] / lvlarray.length;
      newopt['level_height'] = parseInt(options['level_height']) + parseInt(options['height_per_level']);
      newopt['height_per_level'] = options['height_per_level'];
      newopt['width_offset'] = (newopt['level_width'] * n) + options['width_offset'];
      newopt['num_voids'] = lvlarray[n]['children'].length * 2;
      newopt['void_length'] = newopt['level_width'] / newopt['num_voids'];      
      this.generate_all_nodes(newopt, lvlarray[n]['children']);
    }
    
  };

  //-----------------------------------------------------------------------------------------------  
  // generate a node for a given rawtree element
  // raw_node: an element from rawtree
  // options: a named array of options to effect the look of the node created
  this.generate_node = function(raw_node, options) {
    
    //console.warn('Raw Node: '+dump(raw_node));
    if (typeof(raw_node) == 'undefined') { throw new Error("Node to be generated is undefined."); }
    
    // if options not supplied; set defaults
    if (typeof(options['rect_color']) == 'undefined') {
      options['rect_color'] = this.rect_color;
    }
    if (typeof(options['rect_stroke_color']) == 'undefined') {
      options['rect_stroke_color'] = this.rect_stroke_color;
    }
    
    // draw text
    //if ( typeof(raw_node['prime_parent_node']) == 'undefined') {
      var x_coord = (options['void_length'] * options['cur_void']) - 10 + options['width_offset'];
    //} else {
      //var parent_node = this.nodes[ raw_node['prime_parent_node'] ];
      //var x_coord = parent_node[0].attr('x');
    //}
    var node_txt = this.canvas.text(x_coord, options['level_height'], raw_node['name'] );

    // draw the rectangle around the text
    var rect_x = x_coord - (node_txt.getBBox().width / 2) - this.node_padding;
    var rect_y = options['level_height'] - (node_txt.getBBox().height / 2) - this.node_padding;
    var rect_width = node_txt.getBBox().width + (this.node_padding * 2);
    var rect_height = node_txt.getBBox().height + (this.node_padding * 2);
    var node_rect = this.canvas.rect(rect_x, rect_y, rect_width, rect_height, 2).attr({'fill': options['rect_color'], stroke: options['rect_stroke_color']});

    // move text to front
    node_txt.toFront();

    // add onclick event to rectangle to navigate to link
    node_rect.node.onmouseover = function() { this.style.cursor = 'pointer'; }
    node_txt.node.onmouseover = function() { this.style.cursor = 'pointer'; }
    node_rect.node.onclick = function() { window.location.href = raw_node['link']; } 
    node_txt.node.onclick = function() { window.location.href = raw_node['link']; } 
    
    this.add_focus_bubble(node_rect, "Name:"+raw_node['name'],node_txt);
    
    // create and populate set container to hold all node parts
    var node = this.canvas.set(node_txt, node_rect);
    this.nodes.push(node);
    var node_id = this.nodes.length - 1;
    raw_node['node'] = node_id;
 
    // add reference to parent node in each of the children
    // this is so that shared nodes/children can be positioned based on parents
    if (typeof(raw_node['children']) == 'undefined') { throw new Error("Stock Node where name="+raw_node['name']+" has no children's array. At the least this should be an empty array. Stock Node:"+dump(raw_node)); }
    if (raw_node['children'].length > 0) {
      for (c in raw_node['children']) {
        raw_node['children'][c]['prime_parent_node'] = node_id;
      }
    } 
 
    // add reference to parent node in each of the shared children
    // this is so that shared nodes/children can be positioned based on parents
    // also reposition shared nodes at this point since both parents now have nodes drawn
    if (typeof(raw_node['shared_children']) != 'undefined') {
      if (raw_node['shared_children'].length > 0) {
        for (c in raw_node['shared_children']) {
          var child_node = this.get_rawnode(raw_node['shared_children'][c]['location']);
          child_node['addt_parent_node'] = node_id;
          var new_x_coord = this.nodes[child_node['prime_parent_node']][1].attr('x') + (this.nodes[child_node['addt_parent_node']][1].attr('x') - this.nodes[child_node['prime_parent_node']][1].attr('x') ) / 2;
          var x_coord_0 = new_x_coord + (this.nodes[child_node['node']][0].getBBox().width / 2) + this.node_padding;
          this.nodes[child_node['node']][0].attr('x', x_coord_0);
          var x_coord_1 = new_x_coord;
          this.nodes[child_node['node']][1].attr('x', x_coord_1);
          
          // also move child below both it's parents
          if (this.nodes[child_node['node']][1].attr('y') < options['level_height']) {
            this.nodes[child_node['node']][0].attr('y', options['level_height']+options['height_per_level']);
            this.nodes[child_node['node']][1].attr('y', options['level_height']+options['height_per_level'] - (this.node_padding * 2));
          }
        }
      }
    } 
   
    
    return node;
    
  };

  //-----------------------------------------------------------------------------------------------
  // generate an array of relationship colors
  // options is an array
  //  h_start, h_end, h_step
  //  s_start, s_end, s_step
  //  b_start, b_end, b_step
  this.generate_relationship_colors = function(options) {
    // set defaults
    if (typeof(options) == 'undefined') { options = new Array(); }
    if(typeof(options['h_start']) == 'undefined') { options['h_start'] = 0; }
    if(typeof(options['h_end']) == 'undefined') { options['h_end'] = 359; }
    if(typeof(options['h_step']) == 'undefined') { options['h_step'] = 50; }
    if(typeof(options['s_start']) == 'undefined') { options['s_start'] = 100; }
    if(typeof(options['s_end']) == 'undefined') { options['s_end'] = 101; }
    if(typeof(options['s_step']) == 'undefined') { options['s_step'] = 5; }
    if(typeof(options['b_start']) == 'undefined') { options['b_start'] = 50; }
    if(typeof(options['b_end']) == 'undefined') { options['b_end'] = 51; }
    if(typeof(options['b_step']) == 'undefined') { options['b_step'] = 5; }

    // create ranges
    var hues = new Array();
    for (h=options['h_start']; h<=options['h_end']; h=h+options['h_step']) {
      hues.push(h);    
    }
    
    var sats = new Array();
    for (s=options['s_start']; s<=options['s_end']; s=s+options['s_step']) {
      sats.push(s);    
    }
    var brights = new Array();
    for (b=options['b_start']; b<=options['b_end']; b=b+options['b_step']) {
      brights.push(b);    
    }
    
    // get colors
    this.relationship_color_list = new Array();
    for (b in brights) {
      for (s in sats) {
        for (h in hues) {
          var rgb = Raphael.hsb2rgb(hues[h],sats[s],brights[b]);
          this.relationship_color_list.push(rgb.hex);
        }
      }
    }
    
    this.current_rel_index = 0;
    
  };
  
  //-----------------------------------------------------------------------------------------------
  // get the next color in line
  this.get_relationship_color = function () {
    if (typeof(this.relationship_color_list) == 'undefined') {
      this.generate_relationship_colors();
    }
    
    var color = this.relationship_color_list[this.current_rel_index];
    this.current_rel_index = this.current_rel_index + 1;
    return color;
  };
  
  var info_box = this.info_box;
  hide_info_box = function() {
    info_box.hide();
  };
  
  var highlight;
  var highlight_time;
  remove_highlight = function () {
    highlight.remove();
  };
  set_highlight = function(obj) {
    if (typeof(highlight) != 'undefined') {
      highlight.remove();
    }
    highlight = obj;
    highlight.attr({'stroke':info_box_icon, 'stroke-width':5});
    highlight.toBack();
    //highlight.animate({'stroke':info_box_icon, 'stroke-width':3}, 2000, 'elastic');
    //info_box[0].animate({'stroke':info_box_icon}, 2000, 'elastic', function() { console.warn('Orig:'+this.attr('stroke')); this.attr({'stroke':info_box_stroke}); console.warn('Stroke:'+info_box_stroke);} );
    highlight_time = setTimeout("remove_highlight();",2000);
    
  };
  
  //-----------------------------------------------------------------------------------------------
  // Generate a tooltip-like focus bubble
  this.add_focus_bubble = function (obj, text, hover_obj) {
    if (typeof(this.info_box) == 'undefined') {
      this.draw_info_box();          
    }
    
    if (typeof(hover_obj) == 'undefined') { hover_obj = obj; }
    hover_obj.hover(function () {
      info_box.show();
      info_box[2].attr('text', text);
      set_highlight(obj.clone());
      console.warn('TXT:'+info_box[2].attr('text'));
      //setTimeout("hide_info_box()",20000);
    } );  
  };
  
  
  //-----------------------------------------------------------------------------------------------
  // generate all connections between nodes
  this.generate_all_connections = function (options, lvlarray) {
    
    if (typeof(lvlarray) == 'undefined') {
      lvlarray = this.rawtree;
    }
    
    if (typeof(options) == 'undefined') {
      options = new Array();
    }
    
    // connect nodes in current level to their children
    for (n in lvlarray) {
      if (lvlarray[n]['children'].length > 0) {
        // deal with primary children
        for (m in lvlarray[n]['children']) {
          //connect current node to the current child
          var child = lvlarray[n]['children'][m];
          this.generate_connection(lvlarray[n], lvlarray[n]['children'][m],options);
        }
        //generate all connections the next level
        this.generate_all_connections(options, lvlarray[n]['children']);
      }

    } //end of generate connection for node n and children
    
    for (n in lvlarray) {
      if (typeof(lvlarray[n]['shared_children']) != 'undefined') {
        var parent = lvlarray[n];
        //deal with "shared" children
        for (m in parent['shared_children']) {
          //connect current node to the current child
          var child = this.get_rawnode(parent['shared_children'][m]['location']);
          this.generate_connection(parent, child, options, parent['shared_children'][m]['relationship_type']);
          
        }        
      }     
    }
    
  };
  
  //-----------------------------------------------------------------------------------------------
  // generate a connection between two nodes
  this.generate_connection = function (parent, child, options, relationship_type) {

    //console.warn('Generating Connection between '+parent['name']+' => '+child['name']+":\nParent is "+dump(parent)+'Child is '+dump(child));

    // determine coords for connection to start at parent node    
    parent_node = this.nodes[parent['node']][1];
    parent_x = parent_node.attr('x') + (parent_node.getBBox().width / 2);
    parent_y = parent_node.attr('y') + parent_node.getBBox().height;
          
    //determine coords for connection to end at child node
    child_node = this.nodes[child['node']][1];    
    child_x = child_node.attr('x') + (child_node.getBBox().width / 2);;
    child_y = child_node.attr('y');
    
    // draw connection
    var path = 'M ' + parent_x + ' ' +  parent_y;
    path = path + ' L ' + child_x + ' ' + child_y;
    path_obj = this.canvas.path(path);
    
    // set attributes
    if (typeof(relationship_type) == 'undefined') {
      relationship_type = child['relationship_type'];
    }
    if ( typeof(this.relationship_colors[ relationship_type ]) == 'undefined' ) {
      this.relationship_colors[ relationship_type ] = this.get_relationship_color();
      this.relationships.push(relationship_type);
    }
    var relationship_color = this.relationship_colors[ relationship_type ];
    path_obj.attr({'stroke': relationship_color, 'stroke-width': 2});
    
    // set line type based on relationship type
    if (relationship_type != 'is_maternal_parent_of' && relationship_type != 'is_paternal_parent_of') {
      path_obj.attr('stroke-dasharray', '-');
    }
    
    /**
    var label_txt = relationship_type.replace(/[_-]/g, "\n");
    var label_x = Math.abs( (child_x - parent_x) / 2 );
    if (child_x < parent_x) { label_x = label_x + child_x; } else { label_x = label_x + parent_x; }
    var label_y = Math.abs( (child_y - parent_y) / 2 );
    if (child_y < parent_y) { label_y = label_y + child_y; } else { label_y = label_y + parent_y; }
    var label_txt = this.canvas.text(label_x, label_y, label_txt).attr({fill: 'grey'});
    
    var rect_x = label_x - (label_txt.getBBox().width / 2) - 2;
    //var rect_x = 0 - (label_txt.getBBox().width / 2) - 2;
    var rect_y = label_y - (label_txt.getBBox().height / 2) - 2;
    //var rect_y = 0 - (label_txt.getBBox().height / 2) - 2;
    var rect_width = label_txt.getBBox().width + 4;
    var rect_height = label_txt.getBBox().height + 4;
    var label_rect = this.canvas.rect(rect_x, rect_y, rect_width, rect_height, 10).attr({'fill': this.rel_fill_color, 'stroke':this.rel_stroke_color});
    
    label_txt.toFront();
        
    var label = this.canvas.set(
      label_rect,
      label_txt
    );
    path_obj.tooltip(label);
    */
    
    var connection = this.canvas.set(path_obj);
    this.connections.push(path_obj);

    // add relationship type label tooltip
    this.add_focus_bubble(path_obj, parent['name'] + "\n" + relationship_type.replace(/[_-]/g, " ") + "\n" + child['name']);
    
    //Add connections back to parent/child
    if (typeof(parent['outgoing_connections']) == 'undefined') {
      parent['outgoing_connections'] = new Array();
    }
    parent['outgoing_connections'].push(this.connections.length - 1);
    
    if (typeof(child['incomming_connections']) == 'undefined') {
      child['incomming_connections'] = new Array();
    }
    child['incomming_connections'].push(this.connections.length - 1);   
    
    return connection;
  };


  // creates a text element, get's it's length and then removes it before returning the length
  this.sort_relationships_by_length = function () {
    this.relationships.sort( 
      function(a,b) { 
        var element = this.canvas.text(0,0,a);
        var a_length = element.getBBox().width;
        element.remove();
        var element = this.canvas.text(0,0,b);
        var b_length = element.getBBox().width;
        element.remove();
        return b_length - a_length; 
      } 
    );
  };
  
  
  // where x,y are where you want the text placed
  // and align[0] is either “left” or "right” (default is left)
  //      and specifies which side of the key the supplied x coordinate indicates
  // align[1] is either 'top' or 'bottom' (default is top) 
  //      and specifies whether the y coordinate supplied indicates the top or bottom of the key
  this.print_relationship_color_key = function(x,y, align) {
    var box_size = 8;
    var padding = 3;
    var key_padding = 10;
    
    var y_txt = y;
    var txt_objs = this.canvas.set();
    var x_txt = this.canvas.width;
    var box_objs = this.canvas.set();
    var total_key_height = 0;
    var total_key_width = 0;
    
    // sort relationship_colors by length of key
    this.sort_relationships_by_length;
    for(k in this.relationships) {
      n = this.relationships[k];
      // By default aligned at top
      // this will be changed below if align[1] == 'bottom'
      if (align[0] == 'right') {
        // align[0] == right
        // the user has specified the x coordinate that they want the key to end at
        // the text is still start aligned so that the boxes are aligned one on top of the other
        // but the longest text should end at the x coordinate provided
        var txt_obj = this.canvas.text(x_txt,y_txt,n).attr('text-anchor','start');
        if ( x_txt == this.canvas.width) {
          x_txt = x - txt_obj.getBBox().width;
          txt_obj.attr('x',x_txt);
        }
        txt_objs.push(txt_obj);
        var x_box = x_txt - box_size - padding;
        var y_box = y_txt - (box_size/2);
        var box_obj = this.canvas.rect(x_box, y_box, box_size, box_size, 2).attr({'fill':this.relationship_colors[n],'stroke':this.relationship_colors[n]});      
        box_objs.push(box_obj);
      } else {
        // align[0] == 'left' (Default)
        // the user has specified the x coordinate that they want the key to start at
        // thus the boxes start at the x coordinate provided and the text after that
        x_txt = x + box_size + padding;
        var txt_obj = this.canvas.text(x_txt,y_txt,n).attr('text-anchor','start');
        txt_objs.push(txt_obj);
        var x_box = x;
        var y_box = y_txt - (box_size/2);
        var box_obj = this.canvas.rect(x_box, y_box, box_size, box_size, 2).attr({'fill':this.relationship_colors[n],'stroke':this.relationship_colors[n]});
        box_objs.push(box_obj);
      }
      // get largest width
      if (total_key_width < (txt_obj.getBBox().width + box_size + padding)) {
        total_key_width = txt_obj.getBBox().width + box_size + padding;
      }
      
      // get next height
      y_txt = y_txt + txt_obj.getBBox().height;
    }
    total_key_height = y_txt - y;
    
    // Draw the box around the key, etc.
    var key_y;
    var key_x;
    var key_obj = this.canvas.set();
    if (align[0] == 'right') { key_x = x - total_key_width; } else { key_x = x; }
    if (align[1] == 'bottom') { key_y = y - total_key_height; } else { key_y = y; }
    // adjust height to take into account y of txt is middle whereas y of rect is absolute
    key_y = key_y - (txt_obj.getBBox().height/2);
    // adjust width/height based on key_padding
    key_y = key_y - (key_padding/2);
    key_x = key_x - (key_padding/2);
    key_rect = this.canvas.rect(key_x, key_y, total_key_width + key_padding, total_key_height + key_padding);
    key_rect.attr({'fill':this.key_color,'stroke':this.key_outline_stroke});
    key_txt = this.canvas.text(key_x, key_y, "Relationship\nKey");
    var key_txt_y = key_y - (key_txt.getBBox().height/2) - (key_padding/2);
    var key_txt_x = key_x + (key_rect.getBBox().width/2);
    key_txt.attr({'x':key_txt_x,'y':key_txt_y});
    key_rect_y = key_y - key_txt.getBBox().height - key_padding;
    key_top_rect = this.canvas.rect(key_x, key_rect_y, total_key_width + key_padding, key_txt.getBBox().height + key_padding);
    key_top_rect.attr({'fill':this.key_header_color,'stroke':this.key_outline_stroke});
    key_txt.toFront();
    
    
    // if the user has indicates they supplied the bottom y coordinate
    // move the entire key from y = top to y = bottom
    // this is done at this point because the entire size of the key is needed
    // before it can be re-aligned
    if (align[1] == 'bottom') {
      for (var i=0; i < txt_objs.length; i++) {
        var cur_txt_y = txt_objs[i].attr('y');
        txt_objs[i].attr('y', (cur_txt_y - total_key_height));
        var cur_box_y = box_objs[i].attr('y');
        box_objs[i].attr('y', (cur_box_y - total_key_height));
      }
    }
    
    // move all the key contents to the front
    for (var i=0; i < txt_objs.length; i++) {
      txt_objs[i].toFront();
      box_objs[i].toFront();
    }    
    
  };
  
  return this;
}