/*
 * (C) Copyright 2013 Marc Schafer
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Marc Schafer
 */
#include "foamcutapp.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("mit");
    QCoreApplication::setOrganizationDomain("mit.edu");
    QCoreApplication::setApplicationName("FoamCut");
    FoamcutApp a(argc, argv);
    return a.exec();
}
