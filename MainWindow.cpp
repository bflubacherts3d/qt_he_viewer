#include "MainWindow.h"

MainWindow::MainWindow() : QMainWindow() {
    
    _hps_widget = new ts3d::HPSWidget(this);
    _bom_table_widget = new BOMTableWidget(this);

    auto splitter = new QSplitter(Qt::Orientation::Vertical, this);
    splitter->addWidget(_hps_widget);
    splitter->addWidget(_bom_table_widget);
    splitter->setSizes({ size().height() * 2/3, size().height() / 3 });
    
    setCentralWidget(splitter);
    
    auto fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Open", this, &MainWindow::onFileOpen);
    fileMenu->addAction("Close", this, &MainWindow::onFileClose);
}

MainWindow::~MainWindow( void ) {
    
}

void MainWindow::onFileOpen() {
    static QStringList const filters = {
        "3D Studio Files (*.3ds)",
        "3DXML Files (*.3dxml)",
        "ACIS SAT Files (*.sat *.sab)",
        "CADDS Files (*_pd)",
        "CATIA V4 Files (*.model *.dlv *.exp *.session)",
        "CATIA V5 Files (*.CATPart *.CATProduct *.CATShape *.CATDrawing)",
        "CGR Files (*.cgr)",
        "Collada Files (*.dae)",
        "Creo/ProE Files (*.prt *.prt.* *.neu *.neu.* *.asm *.asm.* *.xas *.xpr)",
        "I-DEAS Files (*.arc *.unv *.mf1 *.prt *.pkg)",
        "IFC Files (*.ifc *.ifczip)",
        "IGES Files (*.igs *.iges)",
        "Inventor Files (*.ipt *.iam)",
        "JT Files (*.jt)",
        "KMZ Files (*.kmz)",
        "NX Unigraphics Files (*.prt)",
        "PDF Files (*.pdf)",
        "PRC Files (*.prc)",
        "Parasolid Files (*.x_t *.xmt *.x_b *.xmt_txt)",
        "Rhino Files (*.3dm)",
        "STEP Files (*.stp *.step *.stpz *.stp.z)",
        "SolidEdge Files (*.par *.asm *.pwd *.psm)",
        "SolidWorks Files (*.sldprt *.sldasm *.sldfpp *.asm)",
        "Universal 3D Files (*.u3d)",
        "VDA Files (*.vda)",
        "VRML Files (*.wrl *.vrml)",
        "XVL Files (*.xv3 *.xv0)",
        "All Files (*.*)"
    };
    auto const filename = QFileDialog::getOpenFileName(this, tr("Open File"), ".", filters.join(";;") );
    if( filename.isEmpty() ) {
        return;
    }
    
    // close current file
    
    // open new file
}

void MainWindow::onFileClose() {
    
}
