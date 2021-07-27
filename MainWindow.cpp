#include <A3DSDKIncludes.h>
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
    onFileClose();
    
    // open new file
    A3DRWParamsLoadData load_params;
    A3D_INITIALIZE_DATA(A3DRWParamsLoadData, load_params);
    load_params.m_sGeneral.m_eReadGeomTessMode = kA3DReadTessOnly;
    load_params.m_sGeneral.m_bReadSolids = true;
    A3DAsmModelFile *model_file = nullptr;
    auto const load_result = A3DAsmModelFileLoadFromFile(qPrintable(filename), &load_params, &model_file);
    if(A3D_SUCCESS != load_result) {
        QString error_mesage = A3DMiscGetErrorMsg(load_result);
        QMessageBox::warning(this, "Load Error", QString("Unable to load the specified file. ") + error_mesage);
        return;
    }
    
    static_cast<Application*>(qApp)->setModelFile(model_file);
    
    SceneGraphBuilder builder(model_file);
    
    auto hps_model = HPS::Factory::CreateModel();
    hps_model.GetSegmentKey().IncludeSegment(builder.build());
    _hps_widget->getView().AttachModel(hps_model);
    
    HPS::CameraKit camera;
    _hps_widget->getView().ComputeFitWorldCamera(camera);
    _hps_widget->getView().SmoothTransition(camera);
    
    _bom_table_widget->refresh();
}

void MainWindow::onFileClose() {
    static_cast<Application*>(qApp)->setModelFile(nullptr);
    
    auto hps_model = _hps_widget->getView().GetAttachedModel();
    if(hps_model.Type() == HPS::Type::Model) {
        _hps_widget->getView().DetachModel();
        hps_model.Delete();
        _hps_widget->getView().Update();
    }

    _bom_table_widget->refresh();
}
