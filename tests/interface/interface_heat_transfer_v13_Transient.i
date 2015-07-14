[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 16
  xmax = 0.1
  ymax = 0.01
[]

[MeshModifiers]
  [./atc_region]
    type = SubdomainBoundingBox
    bottom_left = '0.0125 0 0'
    top_right = '0.0875 0.01 1' # {0.1*14/16},{0.01*1},0
    block_name = atc_region # {0.1*2/16},0,0
    block_id = 1
  [../]
  [./atc_nodeset]
    type = BoundingBoxNodeSet
    top_right = '0.0875 0.01 1' # {0.1*14/16},{0.01*1},0
    new_boundary = atc_nodeset
    bottom_left = '0.0125 0 0' # {0.1*2/16},0,0
  [../]
[]

[Variables]
  [./temp]
    initial_condition = 10
  [../]
[]

[Kernels]
  [./hc]
    type = HeatConduction
    variable = temp
    block = 0
  [../]
  [./hc_dt]
    type = HeatConductionTimeDerivative
    variable = temp
  [../]
[]

[BCs]
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
  [./multiscale_DirichletBC]
    type = MultiscaleDirichletBC
    variable = temp
    boundary = atc_nodeset
    lammps_userobject = lammps_uo
  [../]
[]

[Materials]
  active = 'steel dummy_mat'
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
  [./steel]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'thermal_conductivity specific_heat density'
    prop_values = '1.8 100.466 8000' # W/m*K, J/kg-K, kg/m^3 @ 296K
  [../]
  [./dummy_mat]
    type = GenericConstantMaterial
    block = 1
  [../]
[]

[UserObjects]
  [./lammps_uo]
    type = LammpsUserObject
    lammpsInput = /Users/sthomas/lammps/examples/COUPLE/simple/in.lj
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

