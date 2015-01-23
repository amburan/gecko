[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 16
  xmax = 16
  ymax = 3
  zmax = 3
[]

[MeshModifiers]
  [./atc_region]
    type = SubdomainBoundingBox
    bottom_left = '2 0 0'
    top_right = '14 3 3'
    block_name = atc_region
    block_id = 1
  [../]
  [./md_region]
    type = AddExtraNodeset
    new_boundary = md_region
    top_right = '14 3 3'
    bottom_left = '2 0 0'
  [../]
[]

[Variables]
  [./temp]
    initial_condition = 10
  [../]
[]

[Kernels]
  active = 'hc dt'
  [./ms_hc]
    type = MultiscaleHeatConduction
    variable = temp
    block = atc_region
    lammps_userobject = lammps_uo
  [../]
  [./dt]
    type = TimeDerivative
    variable = temp
  [../]
  [./hc]
    type = HeatConduction
    variable = temp
    block = '0 1'
  [../]
[]

[BCs]
  active = 'right_DC left_DC mdRegion_DBC'
  [./left_DC]
    type = DirichletBC
    variable = temp
    boundary = left
    value = 50
  [../]
  [./right_DC]
    type = DirichletBC
    variable = temp
    boundary = right
    value = 10
  [../]
  [./top_ms_DC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = top
    lammps_userobject = lammps_uo
  [../]
  [./bottom_ms_DC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = bottom
    lammps_userobject = lammps_uo
  [../]
  [./mdRegion_DBC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = md_region
    lammps_userobject = lammps_uo
  [../]
[]

[Materials]
  [./hcm]
    type = HeatConductionMaterial
    block = 0
    specific_heat = 1
    thermal_conductivity = 1
  [../]
  [./ms_hc_mat]
    type = HeatConductionMaterial
    block = 1
    specific_heat = 1
    thermal_conductivity = 1
  [../]
[]

[UserObjects]
  [./lammps_uo]
    type = LammpsUserObject
    lammpsInput = /home/sthomas/lammps/lammps-ro/examples/COUPLE/simple/in.bar1d_flux
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 14
[]

[Outputs]
  [./out]
    type = Exodus
    output_initial = true
  [../]
[]

