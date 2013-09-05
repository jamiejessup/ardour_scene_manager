/*
This file is part of Ardour Scene Manager.
Copyright 2013, Jamie Jessup

Ardour Scene Manager is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ardour Scene Manager is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ardour Scene Manager. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ArdourSession.h"

ArdourSessionParser::ArdourSessionParser() {

}

void ArdourSessionParser::init(string fn, Scene* pScene) {
	//clear all of the tracks in the current scene
	pScene->tracks.clear();

	xmlDoc *doc = NULL;
	xmlNode *root_element = NULL;

	/*
	 * this initialize the library and check potential ABI mismatches
	 * between the version it was compiled for and the actual shared
	 * library used.
	 */LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	doc = xmlReadFile(fn.c_str(), NULL, 0);

	if (doc == NULL) {
		printf("error: could not parse file %s\n", fn.c_str());
	}

	/*Get the root element node */
	root_element = xmlDocGetRootElement(doc);

	parseNodes(root_element, pScene);

	/*free the document */
	xmlFreeDoc(doc);

	/*
	 *Free the global variables that may
	 *have been allocated by the parser.
	 */
	xmlCleanupParser();

}

ArdourSessionParser::~ArdourSessionParser() {
	// TODO Auto-generated destructor stub
}

void ArdourSessionParser::parseNodes(xmlNode * a_node, Scene *pScene) {
	xmlNode *cur_node = NULL;
	string name;
	char gain;
	unsigned char *property;

	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			/*
			 * Check if we are a Processor with parent as a Route
			 * (otherwise we could be an aux send)
			 * Aux sends will be implemented later
			 * They are processors with processors as parents, the same otherwise
			 */

			if (strcmp((char*) cur_node->name, "Processor") == 0
					&& strcmp((char*) cur_node->parent->name, "Route") == 0) {
				//	Check if we are in a processor of type "Amp"

				if (strcmp((char*) xmlGetProp(cur_node, BAD_CAST "name"), "Amp")
						== 0) {
					//Get the gain of the track from the Controllable attribute
					property = xmlGetProp(xmlLastElementChild(cur_node),
							BAD_CAST "value");
					if (property != NULL) {
						//Convert gain to db, what we use internally
						gain = absToCC(atof((char*) property));
						//Are we the master track?
						property = xmlGetProp(cur_node->parent,
								BAD_CAST "flags");
						if (property == NULL) {
							pScene->tracks.push_back(Track(gain));
						} else if (strcmp((char*) property, "MasterOut") == 0) {
							pScene->master.setTrackGain(gain);
						}

					}

				}
			}
		}
		parseNodes(cur_node->children, pScene);
	}
}

float ArdourSessionParser::absTodB(float gain) {
	if (gain == 0) {
		return -600;
	} else
		return 20 * log10(gain);
}

float ArdourSessionParser::dBToCC(float gain) {
	//Make up gain to make highest MIDI CC return +6db, and 99 return 0dB
	static float makeUpGain = 6 / log10(127 / (float) 99.0);
	return 99*pow(10,(gain/makeUpGain));
}

char ArdourSessionParser::absToCC(float gain) {
	return (char) dBToCC(absTodB(gain));

}