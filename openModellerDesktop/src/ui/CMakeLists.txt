SET(openModellerDesktop_UIS
  omgaboutbase.ui 
  omgalgorithmmanagerbase.ui 
  omgclimateconverterbase.ui 
  omgclimatedataprocessorguibase.ui 
  omgexperimentdesignerbase.ui 
  omgexperimentselectorbase.ui 
  omgexperimentprogressbase.ui
  omggdalconverterbase.ui 
  omggdalrastercontourbase.ui 
  omggdalwarpbase.ui 
  omglayerselectorbase.ui 
  omglayersetmanagerbase.ui 
  omglistwidgetbase.ui 
  omglocalitiesoptionsbase.ui 
  omgmainwindowbase.ui 
  omgmodeltestoptionsbase.ui 
  omgmodeltestsbase.ui 
  omgoptionsbase.ui 
  omgpreanalysisalgorithmmanagerbase.ui
  omgrasterthresholdbase.ui 
  omgsupportedfileformatsbase.ui 
  omgdatafetcherwizardlicensewidgetbase.ui 
  omgdatafetcherwizardsearchwidgetbase.ui 
  omgdatafetcherwizardprogresswidgetbase.ui 
  omgtermsandconditionsbase.ui  
  omgtextfilesplitterbase.ui 
  omgpublishtocataloguebase.ui 
  )

QT4_WRAP_UI(openModellerDesktop_UIS_H ${openModellerDesktop_UIS})

ADD_CUSTOM_TARGET(ui ALL DEPENDS ${openModellerDesktop_UIS_H})
