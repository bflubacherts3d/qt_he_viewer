#include "Application.h"
#include "MainWindow.h"
#include "SceneGraphBuilder.h"

MainWindow::MainWindow() : QMainWindow() {
    
    setWindowTitle("HOOPS Exchange Tutorial");
    
    
    _hps_widget = new ts3d::HPSWidget(this);
    _bom_table_widget = new BOMTableWidget(this);

    auto splitter = new QSplitter(Qt::Orientation::Vertical, this);
    splitter->addWidget(_hps_widget);
    splitter->addWidget(_bom_table_widget);
    splitter->setSizes({ size().height() * 2/3, size().height() / 3 });
    
    setCentralWidget(splitter);
    
    auto fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction("Open", this, &MainWindow::onFileOpen, QKeySequence::Open);
    fileMenu->addAction("Close", this, &MainWindow::onFileClose, QKeySequence::Close);
    
    HPS::DistantLightKit light;
    light.SetDirection(HPS::Vector(0, 0, -1.5f));
    light.SetColor( HPS::RGBAColor( 0.75, 0.75, 0.75, 1. ) );
    light.SetCameraRelative(true);

    _hps_widget->getView().GetSegmentKey().InsertDistantLight(light);
    _hps_widget->getView().GetSegmentKey().GetVisibilityControl().SetLines(true);
    _hps_widget->getView().GetSegmentKey().GetMaterialMappingControl().SetLineColor(HPS::RGBAColor::Black());

    QSettings settings;
    auto const geometry = settings.value("MainWindow/geometry").toByteArray();
    if (geometry.isEmpty()) {
        QRect rect = qGuiApp->primaryScreen()->geometry();
        resize(rect.width() / 3, rect.height() / 3);
    }
    else {
        restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
        restoreState(settings.value("MainWindow/windowState").toByteArray());
    }
}

MainWindow::~MainWindow( void ) {
    
}

void MainWindow::closeEvent(QCloseEvent* event) {
    QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::onFileOpen() {
    static QStringList const filters = {
        "All Files (*.*)",
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
        "XVL Files (*.xv3 *.xv0)"
    };
    auto const filename = QFileDialog::getOpenFileName(this, tr("Open File"), ".", filters.join(";;") );
    if( filename.isEmpty() ) {
        return;
    }
    
    // close current file
    onFileClose();
    
    // open new file


    
    _bom_table_widget->refresh();
}

void MainWindow::onFileClose() {
    
    auto hps_model = _hps_widget->getView().GetAttachedModel();
    if(hps_model.Type() == HPS::Type::Model) {
        _hps_widget->getView().DetachModel();
        hps_model.Delete();
        _hps_widget->getView().Update();
    }

    _bom_table_widget->refresh();
}
