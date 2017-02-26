// quatos-xml-to-wrml.cpp : Defines the entry point for the console application.
//
// version Version 0.7

#include <fstream>
#include <iostream>
#include <string>
#include <cassert>
#include <cmath>

#include "pugixml.hpp"

using namespace std;
using namespace pugi;

ofstream  fout ; 

xml_document doc;

float scale = 10.0;

int debug = 0;

int display_arm = 0;

const char* const list_color[] = { "0 0 1", "0 1 0" , "0 1 1" , "1 0 0" , "1 0 1",  "1 1 0" , "1 1 1" , "0.5 0.5 0",};

string ReplaceAll(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void write_vrml_header()
{
	fout << "#VRML V2.0 utf8" << endl ;
	fout << "#      ^^^^^  this is the MANDATORY header line" << endl ;
	fout << "# Version 0.7" << endl ;
	fout << "# 2015 Frederic Guichard" << endl ;
	fout << "# fred_fr" << endl ;
	fout << "#" << endl ;

}
void write_vrml_axes()
{
	fout << "# axe Y" << endl;
	fout << "DEF nyil Group {" << endl ;
	fout << "children [	" << endl ;
	fout << "Transform {" << endl ;
	fout << "translation 0.5 1 0.0" << endl ;
	fout << "children ["<< endl ;
	fout << "		Shape {" << endl ;
	fout << "		appearance Appearance {" << endl ;
	fout << "	        material Material {}" << endl ;
	fout << "	    		}" << endl ;
	fout << "		geometry Box {" << endl ;
	fout << "			size  1 0.05 0.05" << endl ;
	fout << "	    	}" << endl ;
	fout << "		}" << endl ;
	fout << "	]" << endl ;
	fout << "		}" << endl ;
	fout << "	]" << endl ;
	fout << "}" << endl ;
	fout << "#" << endl ;

	fout << "Transform {" << endl ;
	fout << "   rotation 0 1 0 1.57" << endl ;
	fout << "	translation 0 0 0" << endl ;
	fout << "	children [ USE nyil ]" << endl ;
	fout << "}" << endl ;
	fout << "#" << endl ;

	fout << "Transform {" << endl ;
	fout << "   rotation 0 0 1 -1.57" << endl ;
	fout << "	translation -1 2 0" << endl ;
	fout << "	children [ USE nyil ]" << endl ;
	fout << "}" << endl ;
	fout << "#" << endl ;
}

		
void write_vrml_camera () {
	// for revert z Axe"
    fout << "Viewpoint {" << endl ;
    fout << "        position        0 0 10" << endl ;
    fout << "        orientation     0 0 0 0" << endl ;
    fout << "}" << endl ;
	fout << "#" << endl ;
}

void convert_custom_motors()
{
	xpath_node_set motor = doc.select_nodes("/quatos_configuration/craft/geometry/motor");
    string motor_xy ;
	string comma (",") ;

    for (xpath_node_set::const_iterator it = motor.begin(); it != motor.end(); ++it)
    {
        xpath_node node = *it;
		motor_xy = node.node().text().get() ;
		int arm_num = stoi (node.node().attribute("port").value());

		size_t  pos_comma = motor_xy.find(comma);
	
		float motor_x = stof (motor_xy.substr(0,pos_comma)) ; 
		float motor_y = stof (motor_xy.substr(pos_comma+1)) ;
		float arm_length = sqrt ( motor_x * motor_x + motor_y * motor_y ) * scale;
		float arm_angle = atan2(motor_y,motor_x);
		cout << arm_num << ":      motor_x=" << motor_x << ", motor_y=" << motor_y << ", arm_length=" << arm_length << ", arm_angle=" << arm_angle << endl;
		fout << "DEF Shell=-:-ARM-" << arm_num << " Group {" << endl;
		fout << "children [" << endl;
		fout << "Transform {" << endl;
		fout << "   translation " << motor_y * scale / 2.0 << " 0 " << motor_x * scale / -2.0 << endl;
		fout << "   rotation 0 1 0 " << arm_angle  << endl;
		fout << "   children [" << endl;
		fout << "   Shape {" << endl;
		fout << "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl;
		fout << "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << " }" << endl;
		fout << "	  }" << endl;
		fout << "	]" << endl;
		fout << "}" << endl;
		fout << "]" << endl;
		fout << "}" << endl;
	}

}

void convert_arm(int display_arm)
{
	float arm_length ;
	string frame_type ;

	xpath_node node = doc.select_single_node("quatos_configuration/craft");
	if (node) {
		frame_type = node.node().attribute("config").value();
	}
	cout << "Generate ARM , frame type = " << frame_type << endl << endl;
	if ( frame_type != "custom" ) {
		if ( debug == 1 ) {
			cout << "!= custom" << endl;
		}
		node = doc.select_single_node("/quatos_configuration/craft/distance/motor") ;
		arm_length = stof (node.node().text().get());
		arm_length = arm_length * scale ;
		//fout << "#      arm length " << arm_length << endl ;

		if ( frame_type == "quad_x" ) {
			for (int i=0; i<4; i++) {
				fout <<  "DEF Shell=-:-ARM_" << i << " Group {" << endl ;
				fout <<  "children [" << endl ;
				fout <<  "Transform {" << endl ;
				fout <<  "   translation " << sin(-0.78 + 1.57 * i) * arm_length / 2.0 << " 0 " << cos(-0.78 + 1.57 * i) * arm_length / -2.0 << endl;
				fout <<  "   rotation 0 1 0 " << -0.78 + 1.57 * i << endl ;
				fout <<  "   children[" << endl ;
				fout <<  "   Shape {" << endl ;
				fout <<  "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl ;
				fout <<  "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << "}" << endl ;
				fout <<  "	  }" << endl ;
				fout <<  "	]" << endl ;
				fout <<  "}" << endl ;
				fout <<  "]" << endl ;
				fout <<  "}" << endl ;
			}
		}
		if ( frame_type == "quad_plus" ) {
			for (int i=0; i<4; i++) {
				fout <<  "DEF Shell=-:-ARM_" << i << " Group {" << endl ;
				fout <<  "children [" << endl ;
				fout <<  "Transform {" << endl ;
				fout <<  "   translation " << sin(1.57 * i) * arm_length / 2.0 << " 0 " << cos(1.57 * i) * arm_length / -2.0 << endl;
				fout <<  "   rotation 0 1 0 " << -1.57 + 1.57 * i << endl ;
				fout <<  "   children[" << endl ;
				fout <<  "   Shape {" << endl ;
				fout <<  "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl ;
				fout <<  "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << "}" << endl ;
				fout <<  "	  }" << endl ;
				fout <<  "	]" << endl ;
				fout <<  "}" << endl ;
				fout <<  "]" << endl ;
				fout <<  "}" << endl ;
			}
		}
		if ( frame_type == "hex_x" ) {
			for (int i=0; i<6; i++) {
				//cout << i << endl;
				fout <<  "DEF Shell=-:-ARM_" << i << " Group {" << endl ;
				fout <<  "children [" << endl ;
				fout <<  "Transform {" << endl ;
				fout << "   translation " << sin(-0.53 + 1.05 * i) * arm_length / 2.0 << " 0 " << cos(-0.53 + 1.05 * i) * arm_length / -2.0 << endl;
				fout << "   rotation 0 1 0 " << 2.09 - 1.05 * i << endl;
				fout << "   children[" << endl;
				fout << "   Shape {" << endl;
				fout << "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl;
				fout << "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << "}" << endl;
				fout <<  "	  }" << endl ;
				fout <<  "	]" << endl ;
				fout <<  "}" << endl ;
				fout <<  "]" << endl ;
				fout <<  "}" << endl ;
			}
		}
		if ( frame_type == "hex_plus" ) {
			for (int i=0; i<6; i++) {

				fout <<  "DEF Shell=-:-ARM_" << i << " Group {" << endl ;
				fout <<  "children [" << endl ;
				fout <<  "Transform {" << endl ;
				fout << "   translation " << sin(1.05 * i) * arm_length / 2.0 << " 0 " << cos(1.05 * i) * arm_length / -2.0 << endl;
				fout << "   rotation 0 1 0  " << 1.59 - 1.05 * i << endl;
				fout <<  "   children[" << endl ;
				fout <<  "   Shape {" << endl ;
				fout <<  "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl ;
				fout << "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << "}" << endl;
				fout <<  "	  }" << endl ;
				fout <<  "	]" << endl ;
				fout <<  "}" << endl ;
				fout <<  "]" << endl ;
				fout <<  "}" << endl ;
			}
		}

		if ( frame_type == "octo_x" ) {
			for (int i=0; i<8; i++) {
				fout <<  "DEF Shell=-:-ARM_" << i << " Group {" << endl ;
				fout <<  "children [" << endl ;
				fout <<  "Transform {" << endl ;
				fout << "   translation " << sin(-0.39 + 0.78 * i) * arm_length / 2.0 << " 0 " << cos(-0.39 + 0.78 * i) * arm_length / -2.0 << endl;
				fout << "   rotation 0 1 0  " << 1.98 - 0.78 * i << endl;
				fout << "   children[" << endl;
				fout << "   Shape {" << endl;
				fout << "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl;
				fout << "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << "}" << endl;
				fout <<  "	  }" << endl ;
				fout <<  "	]" << endl ;
				fout <<  "}" << endl ;
				fout <<  "]" << endl ;
				fout <<  "}" << endl ;
			}
		}
		if ( frame_type == "octo_plus" ) {
			for (int i=0; i<8; i++) {
				fout <<  "DEF Shell=-:-ARM_" << i << " Group {" << endl ;
				fout <<  "children [" << endl ;
				fout <<  "Transform {" << endl ;
				fout << "   translation " << sin(0.78 * i) * arm_length / 2.0 << " 0 " << cos(0.78 * i) * arm_length / -2.0 << endl;
				fout << "   rotation 0 1 0  " << 1.59 - 0.78 * i << endl;
				fout << "   children[" << endl;
				fout << "   Shape {" << endl;
				fout << "    	appearance Appearance { material Material { diffuseColor 1 1 1  } }" << endl;
				fout << "		geometry Box { size " << arm_length << " " << arm_length / 50.0 << " " << arm_length / 25.0 << "}" << endl;
				fout <<  "	  }" << endl ;
				fout <<  "	]" << endl ;
				fout <<  "}" << endl ;
				fout <<  "]" << endl ;
				fout <<  "}" << endl ;
			}
		}
			
		} else {
		if (debug == 1) {
			cout << "custom" << endl;
		}
		if (display_arm == 1) {
			convert_custom_motors();
		}
		}
}

void convert_cube()
{
	xpath_node_set cube = doc.select_nodes("/quatos_configuration/craft/mass/cube");
	size_t num_color = 0 ;
	
	cout << "Convert cube :" << endl;

    for (xpath_node_set::const_iterator it = cube.begin(); it != cube.end(); ++it)
    {
        xpath_node node = *it;
		//cout << "\t- debut " << endl;
		float cube_dimx = stof (node.node().attribute("dimy").value()) * scale ;
		//cout << "\t x = " << cube_dimx << endl;
		float cube_dimy = stof (node.node().attribute("dimz").value()) * scale ;
		//cout << "\t x = " << cube_dimy << endl;
		float cube_dimz = stof (node.node().attribute("dimx").value()) * scale ;
		//cout << "\t x = " << cube_dimz << endl;
		float cube_offsetx = stof (node.node().attribute("offsety").value()) * scale ;
		float cube_offsety = stof (node.node().attribute("offsetz").value()) * scale * -1;
		float cube_offsetz = stof (node.node().attribute("offsetx").value()) * scale * -1;
		string cube_name = node.node().attribute("name").value() ; 
		cout << "\t- " << cube_name << endl ;
		cube_name = ReplaceAll(cube_name," ","-");
		fout << "DEF Shell=-:-"<< cube_name << " Group {" << endl;
		fout << "children [" << endl;
		fout << "Transform {" << endl;
		fout << "   translation " << cube_offsetx << " " << cube_offsety << " " << cube_offsetz << endl;
		fout << "     children [" << endl;
		fout << "	  Shape {" << endl;
		fout << "       		 appearance Appearance { material Material { diffuseColor " << list_color[num_color] << " } }" << endl;
		fout << "       		 geometry Box { size " << cube_dimx << " " << cube_dimy << " " << cube_dimz << " }" << endl;
		fout << "	  }" << endl;
		fout << "	]" << endl;
		fout << "}" << endl;
		fout << "]" << endl;
		fout << "}" << endl;
		fout << "#" << endl;
		num_color++ ;
		if ( num_color > 7 ) num_color = 0 ;
	}
}


int main(int argc, char* argv[])
{
	cout << argc << endl;

	if ( argc < 3 ) {
        cout << "\n Quatos-xml-to-wrml version 0.6"<< endl;
        cout << " Please used :  quatos-xml-to-wrml file-input.xml  file-output.wrl"<< endl;
		return -1 ;
         }

	if (argc == 3) {
		display_arm = 1 ;
	}
	if (argc == 4) {
		display_arm = (int) argv[3];
	}



	fout.open(argv[2]); 
	if(!fout)
    {
        cerr<<"Cannot open the output file."<<std::endl;
        return -1;
    }


	//xml_document doc;
 	string frame_type;
    
	xml_parse_result result = doc.load_file(argv[1]);
    
	cout << endl << endl << "Start" << endl << endl ;

    if (result)
    {
		// Write VRML File Header
		cout << endl << "Write VRML File Header" << endl ;
		write_vrml_header();
		// Create ARM boxe
		convert_arm(display_arm);
		// write axes
		write_vrml_axes();
		// Create cube
		convert_cube();
		cout << endl << "write_vrml_camera" << endl;
		write_vrml_camera ();
    } else {
	   return -1;
	}
	fout.close();

	cout << endl << "End " << endl ;

	return 0;
}

